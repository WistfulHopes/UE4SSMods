#include "SymbolResolver.hpp"

#include <PDB.h>
#include <PDB_DBIStream.h>
#include <PDB_RawFile.h>

#include "File/File.hpp"

namespace ModuleManager 
{
	struct Symbol
	{
		std::string name;
		uint32_t rva;
	};
}

ModuleManager::SymbolResolver::SymbolResolver()
{
    wchar_t exe_path_buffer[1024];
    GetModuleFileNameW(GetModuleHandleW(NULL), exe_path_buffer, 1023);

    std::filesystem::path game_exe_path = exe_path_buffer;
    std::filesystem::path game_directory_path = game_exe_path.parent_path();

    std::filesystem::directory_entry pdb = std::filesystem::directory_entry();
    found_pdb = false;
    
    for (const auto& dir_entry : std::filesystem::recursive_directory_iterator(game_directory_path))
    {
        if (dir_entry.path().filename().string().find("Shipping.pdb") != std::string::npos)
        {
            pdb = dir_entry;
            found_pdb = true;
            break;
        }
    }

    if (!found_pdb) return;

    auto pdb_file_handle = std::move(RC::File::open(pdb));
    auto pdb_file_map = pdb_file_handle.memory_map();
    
    auto raw_pdb_file = PDB::CreateRawFile(pdb_file_map.data());
	auto dbi_stream = CreateDBIStream(raw_pdb_file);

    dllBaseAddress = static_cast<LPVOID>(GetModuleHandleW(NULL));

    const PDB::ImageSectionStream imageSectionStream = dbi_stream.CreateImageSectionStream(raw_pdb_file);
    const PDB::ModuleInfoStream moduleInfoStream = dbi_stream.CreateModuleInfoStream(raw_pdb_file);
    const PDB::CoalescedMSFStream symbolRecordStream = dbi_stream.CreateSymbolRecordStream(raw_pdb_file);
	const PDB::PublicSymbolStream publicSymbolStream = dbi_stream.CreatePublicSymbolStream(raw_pdb_file);

    const PDB::ArrayView<PDB::ModuleInfoStream::Module> modules = moduleInfoStream.GetModules();

	{
		const PDB::ArrayView<PDB::HashRecord> hashRecords = publicSymbolStream.GetRecords();
		const size_t count = hashRecords.GetLength();

		symbols.reserve(count);

		for (const PDB::HashRecord& hashRecord : hashRecords)
		{
			const PDB::CodeView::DBI::Record* record = publicSymbolStream.GetRecord(symbolRecordStream, hashRecord);
			if (record->header.kind != PDB::CodeView::DBI::SymbolRecordKind::S_PUB32)
			{
				// normally, a PDB only contains S_PUB32 symbols in the public symbol stream, but we have seen PDBs that also store S_CONSTANT as public symbols.
				// ignore these.
				continue;
			}

			const uint32_t rva = imageSectionStream.ConvertSectionOffsetToRVA(record->data.S_PUB32.section, record->data.S_PUB32.offset);
			if (rva == 0u)
			{
				// certain symbols (e.g. control-flow guard symbols) don't have a valid RVA, ignore those
				continue;
			}

			symbols.insert({ record->data.S_PUB32.name, rva });
		}
	}

    const PDB::GlobalSymbolStream globalSymbolStream = dbi_stream.CreateGlobalSymbolStream(raw_pdb_file);
    const PDB::ArrayView<PDB::HashRecord> hashRecords = globalSymbolStream.GetRecords();
	
	{
		for (const PDB::HashRecord& hashRecord : hashRecords)
		{
			const PDB::CodeView::DBI::Record* record = globalSymbolStream.GetRecord(symbolRecordStream, hashRecord);

			const char* name = nullptr;
			uint32_t rva = 0u;
			if (record->header.kind == PDB::CodeView::DBI::SymbolRecordKind::S_GDATA32)
			{
				name = record->data.S_GDATA32.name;
				rva = imageSectionStream.ConvertSectionOffsetToRVA(record->data.S_GDATA32.section, record->data.S_GDATA32.offset);
			}
			else if (record->header.kind == PDB::CodeView::DBI::SymbolRecordKind::S_GTHREAD32)
			{
				name = record->data.S_GTHREAD32.name;
				rva = imageSectionStream.ConvertSectionOffsetToRVA(record->data.S_GTHREAD32.section, record->data.S_GTHREAD32.offset);
			}
			else if (record->header.kind == PDB::CodeView::DBI::SymbolRecordKind::S_LDATA32)
			{
				name = record->data.S_LDATA32.name;
				rva = imageSectionStream.ConvertSectionOffsetToRVA(record->data.S_LDATA32.section, record->data.S_LDATA32.offset);
			}
			else if (record->header.kind == PDB::CodeView::DBI::SymbolRecordKind::S_LTHREAD32)
			{
				name = record->data.S_LTHREAD32.name;
				rva = imageSectionStream.ConvertSectionOffsetToRVA(record->data.S_LTHREAD32.section, record->data.S_LTHREAD32.offset);
			}
			else if (record->header.kind == PDB::CodeView::DBI::SymbolRecordKind::S_UDT)
			{
				name = record->data.S_UDT.name;
			}
			else if (record->header.kind == PDB::CodeView::DBI::SymbolRecordKind::S_UDT_ST)
			{
				name = record->data.S_UDT_ST.name;
			}

			if (rva == 0u)
			{
				// certain symbols (e.g. control-flow guard symbols) don't have a valid RVA, ignore those
				continue;
			}

			symbols.insert({ name, rva });
		}
	}

	for (const PDB::ModuleInfoStream::Module& module : modules)
	{
		if (!module.HasSymbolStream())
		{
			continue;
		}

		const PDB::ModuleSymbolStream moduleSymbolStream = module.CreateSymbolStream(raw_pdb_file);
		auto temp_symbols = &symbols;

		moduleSymbolStream.ForEachSymbol([temp_symbols, &imageSectionStream](const PDB::CodeView::DBI::Record* record)
			{
				const char* name = nullptr;
				uint32_t rva = 0u;
				if (record->header.kind == PDB::CodeView::DBI::SymbolRecordKind::S_THUNK32)
				{
					if (record->data.S_THUNK32.thunk == PDB::CodeView::DBI::ThunkOrdinal::TrampolineIncremental)
					{
						// we have never seen incremental linking thunks stored inside a S_THUNK32 symbol, but better be safe than sorry
						name = "ILT";
						rva = imageSectionStream.ConvertSectionOffsetToRVA(record->data.S_THUNK32.section, record->data.S_THUNK32.offset);
					}
				}
				else if (record->header.kind == PDB::CodeView::DBI::SymbolRecordKind::S_TRAMPOLINE)
				{
					// incremental linking thunks are stored in the linker module
					name = "ILT";
					rva = imageSectionStream.ConvertSectionOffsetToRVA(record->data.S_TRAMPOLINE.thunkSection, record->data.S_TRAMPOLINE.thunkOffset);
				}
				else if (record->header.kind == PDB::CodeView::DBI::SymbolRecordKind::S_BLOCK32)
				{
					// blocks never store a name and are only stored for indicating whether other symbols are children of this block
				}
				else if (record->header.kind == PDB::CodeView::DBI::SymbolRecordKind::S_LABEL32)
				{
					// labels don't have a name
				}
				else if (record->header.kind == PDB::CodeView::DBI::SymbolRecordKind::S_LPROC32)
				{
					name = record->data.S_LPROC32.name;
					rva = imageSectionStream.ConvertSectionOffsetToRVA(record->data.S_LPROC32.section, record->data.S_LPROC32.offset);
				}
				else if (record->header.kind == PDB::CodeView::DBI::SymbolRecordKind::S_GPROC32)
				{
					name = record->data.S_GPROC32.name;
					rva = imageSectionStream.ConvertSectionOffsetToRVA(record->data.S_GPROC32.section, record->data.S_GPROC32.offset);
				}
				else if (record->header.kind == PDB::CodeView::DBI::SymbolRecordKind::S_LPROC32_ID)
				{
					name = record->data.S_LPROC32_ID.name;
					rva = imageSectionStream.ConvertSectionOffsetToRVA(record->data.S_LPROC32_ID.section, record->data.S_LPROC32_ID.offset);
				}
				else if (record->header.kind == PDB::CodeView::DBI::SymbolRecordKind::S_GPROC32_ID)
				{
					name = record->data.S_GPROC32_ID.name;
					rva = imageSectionStream.ConvertSectionOffsetToRVA(record->data.S_GPROC32_ID.section, record->data.S_GPROC32_ID.offset);
				}
				else if (record->header.kind == PDB::CodeView::DBI::SymbolRecordKind::S_REGREL32)
				{
					name = record->data.S_REGREL32.name;
					// You can only get the address while running the program by checking the register value and adding the offset
				}
				else if (record->header.kind == PDB::CodeView::DBI::SymbolRecordKind::S_LDATA32)
				{
					name = record->data.S_LDATA32.name;
					rva = imageSectionStream.ConvertSectionOffsetToRVA(record->data.S_LDATA32.section, record->data.S_LDATA32.offset);
				}
				else if (record->header.kind == PDB::CodeView::DBI::SymbolRecordKind::S_LTHREAD32)
				{
					name = record->data.S_LTHREAD32.name;
					rva = imageSectionStream.ConvertSectionOffsetToRVA(record->data.S_LTHREAD32.section, record->data.S_LTHREAD32.offset);
				}

				if (rva == 0u)
				{
					// certain symbols (e.g. control-flow guard symbols) don't have a valid RVA, ignore those
					return;
				}

				temp_symbols->insert({ name, rva });
			});
	}
}

void* ModuleManager::SymbolResolver::ResolveSymbol(const char* mangledSymbolName)
{
    if (!symbols.contains(mangledSymbolName)) return nullptr;
    
    return (void*)(symbols[mangledSymbolName] + (int64_t)dllBaseAddress);
}
