/*
  File CRC16					Uint_16	2	The file crc16
  Standard header start tag			String	10	„HeadStart“
  Standard header structure index		Uint_16	2	0x0001
  Standard header minimal compatibility index	Uint_16	2	0x0001
  File type					Uint_32	4	0x00000002
  header size					Uint_32	4	0x0000009E
  File size					Uint_32	4	The file size (data + header)
  File date					String	20	The file generation date
  File description				String	50	Use the project info contained into the data
  Data CRC 16					Uint_16	2	Data crc read into the data
  Specific header start tag			String	10	“SpecStart”
  Specific header structure index		Uint_16	2	0x0002
  Specific header minimal compatibility index	Uint_16	2	0x0002
  Data structure index				Uint_16	2	0x0001
  Specific header size				Uint_32	4	0x00000012
  Specific header info field			Specific header info struct	Specific header size	See after.
  Specific header end tag			String	10	“SpecEnd”
  Header End					String	10	“HeadEnd ”

  The specific header index 2 is defined as follow:

  Field name	Type	Size in bytes	Value
  programVersion				string	9	Concatenation of the version and the build contained into the data.
  moduleType					Uint_8	1	Module type contained into the data
  Device index					Uint_16	2	Device index provided as argument
  Software compatibility index			Uint_16	2	Sw Compatibility index provided as argument
  programSize					Uint_32	4	Size of the data


00000000  06 be 48 65 61 64 53 74  61 72 74 00 00 01 00 01  |..HeadStart.....|
00000010  00 00 00 02 00 00 00 9e  00 04 f0 9e 32 30 32 33  |............2023|
00000020  2d 30 36 2d 31 36 54 31  31 3a 31 36 3a 30 32 00  |-06-16T11:16:02.|
00000030  52 65 63 74 69 66 69 65  72 00 00 00 00 00 00 00  |Rectifier.......|
00000040  00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00  |................|
*
00000060  00 00 37 93 53 70 65 63  53 74 61 72 74 00 00 02  |..7.SpecStart...|
00000070  00 02 00 01 00 00 00 12  56 31 30 30 30 33 00 00  |........V10003..|
00000080  00 0a 00 3f 00 07 00 04  f0 00 53 70 65 63 45 6e  |...?......SpecEn|
00000090  64 00 00 00 48 65 61 64  45 6e 64 00 00 00 e1 54  |d...HeadEnd....T|

*/

#include <cstdint>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>
#include <cstring>
#include <memory>
#include <vector>
#include <map>
#include <linux/can.h>
#include <arpa/inet.h>

#define DEBUG

namespace imbus {
	enum class module_e : uint8_t {
		NotUsed = 0,
		PSC = 1,
		UIM = 2,
		GWM = 3,
		SVR = 4,
		HPS = 5,
		SSM = 6,
		BSM = 7,
		MISC = 8,
		IOM = 9,
		RFM = 10,
		PCM = 11,
		ACM = 12,
		DCC = 13,
		Reserved = 14,
		GEN = 15
	};

	const std::map<module_e, std::string_view> Modulestr_map {
		{ module_e::PSC, "PSU" },
		{ module_e::UIM, "UIM" },
		{ module_e::GWM, "GWM" },
		{ module_e::SVR, "SVR" },
		{ module_e::HPS, "HPS" },
		{ module_e::SSM, "SSM" },
		{ module_e::BSM, "BSM" },
		{ module_e::MISC, "MISC" },
		{ module_e::IOM, "IOM" },
		{ module_e::RFM, "RFM " },
		{ module_e::PCM, "PCM" },
		{ module_e::ACM, "ACM" },
		{ module_e::DCC, "DCC" },
		{ module_e::GEN, "GEN" }
	};

	module_e
	module_type(uint8_t modnbr)
	{
		const module_e mod {static_cast<module_e>(modnbr)};

		switch (mod) {
		case module_e::PSC:
		case module_e::UIM:
		case module_e::GWM:
		case module_e::SVR:
		case module_e::HPS:
		case module_e::SSM:
		case module_e::BSM:
		case module_e::MISC:
		case module_e::IOM:
		case module_e::RFM:
		case module_e::PCM:
		case module_e::ACM:
		case module_e::DCC:
		case module_e::GEN:
			return mod;

		case module_e::NotUsed:
		case module_e::Reserved:
		default:
			return module_e::Reserved;
		}
	}

	std::string_view
	module_type_str(module_e mod)
	{
		auto r {Modulestr_map.find(mod)};
		return r != Modulestr_map.end() ? r->second : std::string_view {"Unknown module type"};
	}

	namespace rfm {
		/* Order follows device index in document, IMBUS_Module-Serial-Number.docx */
		enum class type_e : uint16_t {
			Unknown = 0,
			No_RFM_Connected = 1,
			FR48V_2000_E = 2,
			DPR1200B_48 = 3,
			DPR1500B_48 = 4,
			DPR600B_48 = 5,
			DPR7200B_48 = 6,
			FR48_60_2000 = 7,
			UNKNOWN_24 = 8,
			UNKNOWN_60 = 9,
			DPR600B_60  = 10,
			DPR3500B_48 = 11,
			DPR3500B_24 = 12,
			DPR300B_48 = 13,
			DPR1600B_48 = 14,
			DPR2700B_48 = 15,
			DPR2400B_48 = 16,
			DPR4000B_48 = 17,
			DPR2900B_48 = 18,
			DPR4000B_48_60 = 19,
			DPR850B_48 = 20,
			DPR2000B_48 = 21,
			DPR6000B_48A1 = 22,
			DPR6000B_48B1 = 23,
			DPR48_15B = 24,
			DPR24_100B = 25,
			DPR3000B_48 = 26,
			ESR48_56FF = 27,
			DPR2900F_48A1 = 28,
			DPR4000B_48A5 = 29,
			DPR6000B_48A1_CH = 30,
			DPR2500F_48A1 = 31,
			DPR3000B_48A1 = 32,
			DPR1000B_48 = 33,
			DPR12000_240V = 34,
			DPR12000_336V = 35,
			DPR3000B_48A4 = 36,
			DPR1800B_48 = 37,
			DPR1800B_48A2 = 38,
			DPR6000B_48A2 = 39,
			DPR10000B_48C1_3PH_HV = 40,
			DPR10000B_48C1_3PH_LV = 41,
			DPR2900B_48A7 = 42,
			DPR2000B_48A2_Eltek = 43,
			DPR1800B_48B1 = 44,
			DPR7200B_48C1_3PH_HV = 45,
			DPR7200B_48C1_3PH_LV = 46,
			DPR3500B_48A2 = 47,
			DPR1800E_48D1 = 48,
			DPR3600B_60A2 = 49,
			DPR1080E_48D1 = 50,
			DPR1800B_60VI2 = 51,
			DPR18000B_240 = 52,
			DPC1000B_48A1 = 53,
			DPR2000B_48B1 = 54,
			DPR110V_HVDC_India = 55,
			DPR1800B_48A3 = 56,
			DPR2000B_48A3 = 57,
			Reserved_other_110V = 58,
			Reserved_other_220V = 59,
			Reserved_other_336V = 60,
			DPR11000B_48C1_3PH_HV = 61,
			DPR11000B_48C1_3PH_LV = 62,
			// eDPR2000N_48_IP65_JP = 63,
			DPR2000N_48 = 63,
			DPR3000N_48_3PH_LV_IP65_JP = 64,
			HVR_270_125A_A = 65,
			HVR_336_90A_A = 66,
			DPR3000E_48U1 = 67,
			DPR2900B_48B1 = 68,
			DPR3000N_48 = 69,
			DPC3000N_48 = 69, // DC input
			DPC3000B_48 = 73, // DC input
			DPR1500N_60 = 77, // Lyft PSU 75V
		};

		const std::map<type_e, std::string_view> Typestr_map {
			{ type_e::Unknown,		"Unknown RFM" },
			{ type_e::No_RFM_Connected,	"No RFM connected" },
			{ type_e::FR48V_2000_E,		"FR48V 2000E" },
			{ type_e::DPR1200B_48,		"DPR1200B 48" },
			{ type_e::DPR1500B_48,		"DPR1500B 48" },
			{ type_e::DPR600B_48,		"DPR600B 48" },
			{ type_e::DPR7200B_48,		"DPR7200B 48" },
			{ type_e::FR48_60_2000,		"FR48 60 2000" },
			{ type_e::UNKNOWN_24,		"UNKNOWN 24" },
			{ type_e::UNKNOWN_60,		"UNKNOWN 60" },
			{ type_e::DPR600B_60,		"DPR600B 60 " },
			{ type_e::DPR3500B_48,		"DPR3500B 48" },
			{ type_e::DPR3500B_24,		"DPR3500B 24" },
			{ type_e::DPR300B_48,		"DPR300B 48" },
			{ type_e::DPR1600B_48,		"DPR1600B 48" },
			{ type_e::DPR2700B_48,		"DPR2700B 48" },
			{ type_e::DPR2400B_48,		"DPR2400B 48" },
			{ type_e::DPR4000B_48,		"DPR4000B 48" },
			{ type_e::DPR2900B_48,		"DPR2900B 48" },
			{ type_e::DPR4000B_48_60,	"DPR4000B 48 60" },
			{ type_e::DPR850B_48,		"DPR850B 48" },
			{ type_e::DPR2000B_48,		"DPR2000B 48" },
			{ type_e::DPR6000B_48A1,	"DPR6000B 48A1" },
			{ type_e::DPR6000B_48B1,	"DPR6000B 48B1" },
			{ type_e::DPR48_15B,		"DPR48 15B" },
			{ type_e::DPR24_100B,		"DPR24 100B" },
			{ type_e::DPR3000B_48,		"DPR3000B 48" },
			{ type_e::ESR48_56FF,		"ESR48/56FF" },
			{ type_e::DPR2900F_48A1,	"DPR2900F 48A1" },
			{ type_e::DPR4000B_48A5,	"DPR4000B 48A5" },
			{ type_e::DPR6000B_48A1_CH,	"DPR6000B 48A1_CH" },
			{ type_e::DPR2500F_48A1,	"DPR2500F 48A1" },
			{ type_e::DPR3000B_48A1,	"DPR3000B 48A1" },
			{ type_e::DPR1000B_48,		"DPR1000B 48" },
			{ type_e::DPR12000_240V,	"DPR12000 240V" },
			{ type_e::DPR12000_336V,	"DPR12000 336V" },
			{ type_e::DPR3000B_48A4,	"DPR3000B 48A4" },
			{ type_e::DPR1800B_48,		"DPR1800B 48" },
			{ type_e::DPR1800B_48A2,	"DPR1800B 48A2" },
			{ type_e::DPR6000B_48A2,	"DPR6000B 48A2" },
			{ type_e::DPR10000B_48C1_3PH_HV, "DPR10000B 48C1_3PH_HV" },
			{ type_e::DPR10000B_48C1_3PH_LV, "DPR10000B 48C1_3PH_LV" },
			{ type_e::DPR2900B_48A7,	"DPR2900B 48A7" },
			{ type_e::DPR2000B_48A2_Eltek,	"DPR2000B 48A2_Eltek" },
			{ type_e::DPR1800B_48B1,	"DPR1800B 48B1" },
			{ type_e::DPR7200B_48C1_3PH_HV,"DPR7200B 48C1_3PH_HV" },
			{ type_e::DPR7200B_48C1_3PH_LV,"DPR7200B 48C1_3PH_LV" },
			{ type_e::DPR3500B_48A2,	"DPR3500B 48A2" },
			{ type_e::DPR1800E_48D1,	"DPR1800E 48D1" },
			{ type_e::DPR3600B_60A2,	"DPR3600B 60A2" },
			{ type_e::DPR1080E_48D1,	"DPR1080E 48D1" },
			{ type_e::DPR2000N_48,         "DPR2000N 48" },
			{ type_e::DPR3000N_48,		"DPR3000N 48" },
			{ type_e::DPC3000B_48,		"DPC3000B 48" },
			{ type_e::DPR1500N_60,		"DPR1500N 60" }
		};

		type_e
		device_type(uint16_t device_idx)
		{
			if (device_idx == 0xff)
				return type_e::No_RFM_Connected;

			type_e t {static_cast<type_e>(device_idx)};

			switch (t) {
				// case type_e::UNKNOWN_48:
			case type_e::FR48V_2000_E:      /* 2 */
			case type_e::DPR1200B_48:       /* 3 */
			case type_e::DPR1500B_48:       /* 4 */
			case type_e::DPR600B_48:        /* 5 */
			case type_e::DPR7200B_48:       /* 6 */
			case type_e::FR48_60_2000:      /* 7 */
				// case type_e::UNKNOWN_24:        /* 8 */
				// case type_e::UNKNOWN_60:        /* 9 */
			case type_e::DPR600B_60:        /* 10 */
			case type_e::DPR3500B_48:       /* 11 */
				// case type_e::DPR3500B_24:       /* 12 */
			case type_e::DPR300B_48:        /* 13 */
			case type_e::DPR1600B_48:       /* 14 */
			case type_e::DPR2700B_48:       /* 15 */
			case type_e::DPR2400B_48:       /* 16 */
			case type_e::DPR4000B_48:       /* 17 */
			case type_e::DPR2900B_48:       /* 18 */
			case type_e::DPR4000B_48_60:    /* 19 */
			case type_e::DPR850B_48:        /* 20 */
			case type_e::DPR2000B_48:       /* 21 */
			case type_e::DPR6000B_48A1:     /* 22 */
			case type_e::DPR6000B_48B1:     /* 23 */
			case type_e::DPR48_15B:         /* 24 */
				// case type_e::DPR24_100B:        /* 25 */
			case type_e::DPR3000B_48:       /* 26 */
				// case type_e::ESR48_56FF:        /* 27 */
			case type_e::DPR2900F_48A1:     /* 28 */
				// case type_e::DPR4000B_48A5:     /* 29 */
				// case type_e::DPR6000B_48A1_CH:  /* 30 */
			case type_e::DPR2500F_48A1:     /* 31 */
			case type_e::DPR3000B_48A1:     /* 32 */
			case type_e::DPR1000B_48:       /* 33 */
				// case type_e::DPR12000_240V:     /* 34 */
				// case type_e::DPR12000_336V:     /* 35 */
			case type_e::DPR3000B_48A4:     /* 36 */
			case type_e::DPR1800B_48:       /* 37 */
			case type_e::DPR1800B_48A2:     /* 38 */
				// case type_e::DPR6000B_48A2:         /* 39 */
				// case type_e::DPR10000B_48C1_3PH_HV: /* 40 */
				// case type_e::DPR10000B_48C1_3PH_LV: /* 41 */
			case type_e::DPR2900B_48A7:       /* 42 */
			case type_e::DPR2000B_48A2_Eltek: /* 43 */
				// case type_e::DPR1800B_48B1:       /* 44 */
				// case type_e::DPR7200B_48C1_3PH_HV:/* 45 */
				// case type_e::DPR7200B_48C1_3PH_LV:/* 46 */
				// case type_e::DPR3500B_48A2:       /* 47 */
				// case type_e::DPR1800E_48D1:       /* 48 */
				// case type_e::DPR3600B_60A2:       /* 49 */
				// case type_e::DPR1080E_48D1:       /* 50 */
			case type_e::DPR2000N_48: /* 63 */
			case type_e::DPR3000N_48: /* 69 */
			case type_e::DPC3000B_48: /* 73 */
			case type_e::DPR1500N_60: /* 77 */
				return t;

			default: return type_e::Unknown;
			}
		}

		std::string_view
		device_type_str(type_e t) {
			auto r {Typestr_map.find(t)};
			return r != Typestr_map.end() ? r->second : Typestr_map.at(type_e::Unknown);
		}
	} /* namespace rfm */
}

struct specific_header_t {
	char		prog_version[9];	// Concatenation of the version and the build contained into the data.
	uint8_t		mod_type;		// Module type contained into the data
	uint16_t	dev_idx;		// Device index provided as argument
	uint16_t	software_compatibility_index;	// Sw Compatibility index provided as argument
	uint32_t	prog_size;		// Size of the data
};

struct modsw_header_t {
	uint16_t	file_crc16;
	char 		header_start_tag[10];
	uint16_t 	standard_header_structure_index;		// 0x0001
	uint16_t	standard_header_minimal_compatibility_index;	// 0x0001
	uint32_t	file_type;		// 0x00000002
	uint32_t	header_size;		// 0x0000009E
	uint32_t	file_size;		// The file size (data + header)
	char 		file_date[20];		// The file generation date
	char		file_description[50];	// Use the project info contained into the data
	uint16_t	data_crc16;		// Data CRC read into the data
	char		specific_header_start_tag[10];		// SpecStart
	uint16_t	specific_header_structure_index;	// 0x0002
	uint16_t	specific_header_minimal_compatibility_index;// 0x0002
	uint16_t	data_structure_index;			// 0x0001
	uint32_t	specific_header_size;			// 0x00000012
};

struct modsw_tail_t {
	char		specific_header_end_tag[10];		// SpecEnd
	char		header_end[10];				// HeadEnd
};

static void
print_header(const modsw_header_t &h)
{
	std::cout << "File CRC-16:		" << std::hex << h.file_crc16 << "h\n";
	std::cout << "Header start tag:	" << std::string_view {h.header_start_tag} << std::endl;
	std::cout << "Standard header struct index: 	" << std::hex << static_cast<int>(h.standard_header_structure_index) << std::endl;
	std::cout << "Standard header minimal compatibility index: " << std::hex << h.standard_header_minimal_compatibility_index << std::endl;
	std::cout << "File type:		" << h.file_type << std::endl;
	std::cout << "Header size:		" << std::dec << h.header_size << "-byte\n";
	std::cout << "File size:		" << std::dec << h.file_size << "-byte\n";
	std::cout << "File date/time:		" << std::string_view {h.file_date} << std::endl;
	std::cout << "File description:	" << std::string_view {h.file_description} << std::endl;
	std::cout << "Data CRC-16:		" << std::hex << h.data_crc16 << "h\n";
	std::cout << std::endl;
	std::cout << "Specific header start tag:			" << std::string_view {h.specific_header_start_tag} << std::endl;
	std::cout << "Specific header structure index:		" << h.specific_header_structure_index << std::endl;
	std::cout << "Specific header minimal compatibility index:	" << h.specific_header_minimal_compatibility_index << std::endl;
	std::cout << "Data struct index:		" << h.data_structure_index << std::endl;
	std::cout << "Specific header size:		" << std::dec << h.specific_header_size << "-byte\n";
}

static void
print_specific(const specific_header_t &h)
{
	std::cout << "Program version:		" << std::string_view {h.prog_version} << std::endl;
	std::cout << "Module type:			" << h.mod_type << std::endl;
	std::cout << "Device index:			" << h.dev_idx << std::endl;
	std::cout << "Software compatibility index:	" << std::dec << h.software_compatibility_index << std::endl;
	std::cout << "Programe size:			" << std::dec << h.prog_size << "-byte\n";
}

static void
print_tail(const modsw_tail_t &h)
{
	std::cout << "Specific header end tag:	" << std::string_view {h.specific_header_end_tag} << std::endl;
	std::cout << "Header end:			" << std::string_view {h.header_end} << std::endl;
}

class crc {
public:
	static uint16_t calculate_crc16(const void *pData, unsigned int len, uint16_t crc=0);
	static uint16_t calculate_crc16(unsigned char b, uint16_t crc);
	static uint16_t calculate_crc16(std::istreambuf_iterator<char>); // Till the end
	static uint16_t calculate_crc16(std::istreambuf_iterator<char> start, std::size_t repeat_count);
};

class MODSWheader {
public:
	MODSWheader(const std::filesystem::path&);

	const std::filesystem::path sw_path;
	bool			good() const;
	imbus::module_e		module_type() const;
	std::string_view	module_type_str() const;
	uint16_t		device_idx() const;
	std::string_view	rfm_device_type() const;
	int			sw_compatibility_idx() const;
	std::size_t		sw_size() const;
	std::ifstream		sw_ifstream() const;

private:
	struct impl_t;
	std::unique_ptr<impl_t> mp_Impl;
};

struct MODSWheader::impl_t {
	impl_t(const std::filesystem::path &sw);
	~impl_t() = default;

	const std::filesystem::path file_path;
	modsw_header_t header;
	specific_header_t specific;
	modsw_tail_t tail;
	uint16_t file_crc16 {UINT16_MAX};
	uint16_t data_crc16 {UINT16_MAX};

	constexpr bool		good() const;
	imbus::module_e		module_type() const;
	std::string_view	module_type_str() const;
	constexpr uint16_t	device_idx() const;
	std::string_view	rfm_device_type() const;
	constexpr uint16_t	sw_compatibility_idx() const;
	constexpr std::size_t	sw_size() const;
	std::ifstream		sw_ifstream() const;
};

// MODSWheader
// ---------------------------------------------------------------------
MODSWheader::MODSWheader(const std::filesystem::path &sw)
	: sw_path {sw}, mp_Impl {new impl_t {sw}}
{}

bool MODSWheader::good() const
{ return mp_Impl->good(); }

imbus::module_e MODSWheader::module_type() const
{ return mp_Impl->module_type(); }

std::string_view MODSWheader::module_type_str() const
{ return mp_Impl->module_type_str(); }

uint16_t MODSWheader::device_idx() const
{ return mp_Impl->device_idx(); }

std::string_view MODSWheader::rfm_device_type() const
{ return mp_Impl->rfm_device_type(); }

int MODSWheader::sw_compatibility_idx() const
{ return static_cast<int>(mp_Impl->sw_compatibility_idx()); }

std::ifstream MODSWheader::sw_ifstream() const
{ return mp_Impl->sw_ifstream(); }

std::size_t MODSWheader::sw_size() const
{ return mp_Impl->sw_size(); }

// impl_t
// ---------------------------------------------------------------------
MODSWheader::impl_t::impl_t(const std::filesystem::path &sw)
	: file_path {sw}
{
	if (!std::filesystem::exists(sw)) {
		std::cerr << "MODSWheader: " << sw.string() << " doesn't exist!\n" << std::flush;
		return;
	}

	if (!std::filesystem::is_regular_file(sw)) {
		std::cerr << "MODSWheader: " << sw.string() << " is not a file!\n" << std::flush;
		return;
	}

	std::ifstream f;
	try {
		f.open(sw);
		f.read(reinterpret_cast<char*>(&header), sizeof header);

		header.file_crc16 = ntohs(header.file_crc16);
		header.standard_header_structure_index = ntohs(header.standard_header_structure_index);
		header.standard_header_minimal_compatibility_index = ntohs(header.standard_header_minimal_compatibility_index);
		header.file_type = ntohl(header.file_type);
		header.header_size = ntohl(header.header_size);
		header.file_size = ntohl(header.file_size);
		header.data_crc16 = ntohs(header.data_crc16);
		header.specific_header_structure_index = ntohs(header.specific_header_structure_index);
		header.specific_header_minimal_compatibility_index = ntohs(header.specific_header_minimal_compatibility_index);
		header.data_structure_index = ntohs(header.data_structure_index);
		header.specific_header_size = ntohl(header.specific_header_size);
		print_header(header);

		std::unique_ptr<char[]> buf = std::make_unique<char[]>(header.specific_header_size);
		f.read(reinterpret_cast<char*>(buf.get()), header.specific_header_size);

		char *ptr {buf.get()};
		std::memcpy(specific.prog_version, ptr, sizeof specific.prog_version);
		ptr += sizeof specific.prog_version;
		std::memcpy(&specific.mod_type, ptr, sizeof specific.mod_type);
		ptr += sizeof specific.mod_type;
		std::memcpy(&specific.dev_idx, ptr, sizeof specific.dev_idx);
		specific.dev_idx = ntohs(specific.dev_idx);
		ptr += sizeof specific.dev_idx;
		std::memcpy(&specific.software_compatibility_index, ptr, sizeof specific.software_compatibility_index);
		specific.software_compatibility_index = ntohs(specific.software_compatibility_index);
		ptr += sizeof specific.software_compatibility_index;
		std::memcpy(&specific.prog_size, ptr, sizeof specific.prog_size);
		specific.prog_size = ntohl(specific.prog_size);
		print_specific(specific);

		f.read(reinterpret_cast<char*>(&tail), sizeof tail);
		print_tail(tail);

		/*
		 * Data CRC-16
		 */
		// std::vector<uint8_t> vec {std::istreambuf_iterator<char>(f), {}};

		/*
		 * Last 2-byte is the program CRC-16,
		 * it is appended in tail of the firmware, the data CRC-16 doesn't count it.
		 */
		data_crc16 = crc::calculate_crc16(f, specific.prog_size - sizeof (uint16_t));
		std::cout << "Data CRC-16: " << std::hex << data_crc16 << "h\n";

		/*
		 * Calculate file CRC-16
		 */
		std::ifstream f1 {sw};
		f1.seekg(sizeof header.file_crc16); // Skip first element of the header (File CRC16)
		file_crc16 = crc::calculate_crc16(f1);
		std::cout << "File CRC-16: " << std::hex << file_crc16 << "h\n";
	}
	catch (const std::ifstream::failure &e) {
		std::cout << e.what() << std::endl;
	}
	f.close();
}

constexpr bool MODSWheader::impl_t::good() const
{ return header.file_crc16 == file_crc16 && header.data_crc16 == data_crc16; }

imbus::module_e MODSWheader::impl_t::module_type() const
{ return imbus::module_type(specific.mod_type); }

std::string_view MODSWheader::impl_t::module_type_str() const
{ return imbus::module_type_str(imbus::module_type(specific.mod_type)); }

constexpr uint16_t MODSWheader::impl_t::device_idx() const
{ return specific.dev_idx; }

std::string_view
MODSWheader::impl_t::rfm_device_type() const
{
	return module_type() == imbus::module_e::RFM
		? imbus::rfm::device_type_str(imbus::rfm::device_type(device_idx()))
		: std::string_view {"Not a RFM module"};
}

constexpr uint16_t MODSWheader::impl_t::sw_compatibility_idx() const
{ return specific.software_compatibility_index; }

constexpr std::size_t MODSWheader::impl_t::sw_size() const
{ return specific.prog_size; }

std::ifstream
MODSWheader::impl_t::sw_ifstream() const
{
	if (good()) {
		std::ifstream s {file_path};
		s.seekg(header.header_size);
		return s;
	}
	return std::ifstream {};
}

int
main(int argc, char *argv[])
{
	if (argc != 2) {
		std::cout << "\n\t" << argv[0] << " MODULE-SW-FILE\n\n" << std::flush;
		exit(0);
	}

	std::filesystem::path sw_path {argv[1]};
	MODSWheader h {sw_path};
	std::cout << sw_path << " wellness: " << h.good() << std::endl;
	std::ifstream f {h.sw_ifstream()};
	std::cout << "fstream open state: " << f.is_open() << std::endl;
	std::size_t len {h.sw_size() - sizeof(uint16_t)}; // uint16_t : CRC-16

	uint16_t crc0 {};
	std::istreambuf_iterator<char> start {f};
	for (int i {}; i < len; ++i)
		crc0 = crc::calculate_crc16(static_cast<unsigned char>(*start++), crc0);

	uint16_t crc1 {};
	f.seekg(158); // Rewind ifstream to offset : header size
	crc1 = crc::calculate_crc16(f, len);

	std::cout << "Re-calculated data CRC-16: " << std::hex << crc0 << '\t' << crc1 << std::endl;

	std::cout << "Module type: " << h.module_type_str() << std::endl;
	std::cout << "Device type: " << h.rfm_device_type() << std::endl;
	std::cout << "SW Compatibility Index: " << h.sw_compatibility_idx() << std::endl;
	std::cout << "SW size: " << std::dec << h.sw_size() << "-byte\n" << std::flush;
	return 0;
}

constexpr uint16_t crc16Table[] = {
        0x0000, 0xc0c1, 0xc181, 0x0140, 0xc301, 0x03c0, 0x0280, 0xc241,
        0xc601, 0x06c0, 0x0780, 0xc741, 0x0500, 0xc5c1, 0xc481, 0x0440,
        0xcc01, 0x0cc0, 0x0d80, 0xcd41, 0x0f00, 0xcfc1, 0xce81, 0x0e40,
        0x0a00, 0xcac1, 0xcb81, 0x0b40, 0xc901, 0x09c0, 0x0880, 0xc841,
        0xd801, 0x18c0, 0x1980, 0xd941, 0x1b00, 0xdbc1, 0xda81, 0x1a40,
        0x1e00, 0xdec1, 0xdf81, 0x1f40, 0xdd01, 0x1dc0, 0x1c80, 0xdc41,
        0x1400, 0xd4c1, 0xd581, 0x1540, 0xd701, 0x17c0, 0x1680, 0xd641,
        0xd201, 0x12c0, 0x1380, 0xd341, 0x1100, 0xd1c1, 0xd081, 0x1040,
        0xf001, 0x30c0, 0x3180, 0xf141, 0x3300, 0xf3c1, 0xf281, 0x3240,
        0x3600, 0xf6c1, 0xf781, 0x3740, 0xf501, 0x35c0, 0x3480, 0xf441,
        0x3c00, 0xfcc1, 0xfd81, 0x3d40, 0xff01, 0x3fc0, 0x3e80, 0xfe41,
        0xfa01, 0x3ac0, 0x3b80, 0xfb41, 0x3900, 0xf9c1, 0xf881, 0x3840,
        0x2800, 0xe8c1, 0xe981, 0x2940, 0xeb01, 0x2bc0, 0x2a80, 0xea41,
        0xee01, 0x2ec0, 0x2f80, 0xef41, 0x2d00, 0xedc1, 0xec81, 0x2c40,
        0xe401, 0x24c0, 0x2580, 0xe541, 0x2700, 0xe7c1, 0xe681, 0x2640,
        0x2200, 0xe2c1, 0xe381, 0x2340, 0xe101, 0x21c0, 0x2080, 0xe041,
        0xa001, 0x60c0, 0x6180, 0xa141, 0x6300, 0xa3c1, 0xa281, 0x6240,
        0x6600, 0xa6c1, 0xa781, 0x6740, 0xa501, 0x65c0, 0x6480, 0xa441,
        0x6c00, 0xacc1, 0xad81, 0x6d40, 0xaf01, 0x6fc0, 0x6e80, 0xae41,
        0xaa01, 0x6ac0, 0x6b80, 0xab41, 0x6900, 0xa9c1, 0xa881, 0x6840,
        0x7800, 0xb8c1, 0xb981, 0x7940, 0xbb01, 0x7bc0, 0x7a80, 0xba41,
        0xbe01, 0x7ec0, 0x7f80, 0xbf41, 0x7d00, 0xbdc1, 0xbc81, 0x7c40,
        0xb401, 0x74c0, 0x7580, 0xb541, 0x7700, 0xb7c1, 0xb681, 0x7640,
        0x7200, 0xb2c1, 0xb381, 0x7340, 0xb101, 0x71c0, 0x7080, 0xb041,
        0x5000, 0x90c1, 0x9181, 0x5140, 0x9301, 0x53c0, 0x5280, 0x9241,
        0x9601, 0x56c0, 0x5780, 0x9741, 0x5500, 0x95c1, 0x9481, 0x5440,
        0x9c01, 0x5cc0, 0x5d80, 0x9d41, 0x5f00, 0x9fc1, 0x9e81, 0x5e40,
        0x5a00, 0x9ac1, 0x9b81, 0x5b40, 0x9901, 0x59c0, 0x5880, 0x9841,
        0x8801, 0x48c0, 0x4980, 0x8941, 0x4b00, 0x8bc1, 0x8a81, 0x4a40,
        0x4e00, 0x8ec1, 0x8f81, 0x4f40, 0x8d01, 0x4dc0, 0x4c80, 0x8c41,
        0x4400, 0x84c1, 0x8581, 0x4540, 0x8701, 0x47c0, 0x4680, 0x8641,
        0x8201, 0x42c0, 0x4380, 0x8341, 0x4100, 0x81c1, 0x8081, 0x4040
};

uint16_t
crc::calculate_crc16(const void *pData, unsigned int len, uint16_t crc)
{
	const unsigned char *ptr {reinterpret_cast<const unsigned char*>(pData)};
        while (len-- > 0)
                crc = ((crc >> 8) & 0xFF) ^ crc16Table[(crc ^ *ptr++) & 0xFF];
        return crc;
}

uint16_t crc::calculate_crc16(unsigned char b, uint16_t crc)
{ return ((crc >> 8) & 0xFF) ^ crc16Table[(crc ^ b) & 0xFF]; }

uint16_t
crc::calculate_crc16(std::istreambuf_iterator<char> start)
{
	uint16_t crc {};
	std::istreambuf_iterator<char> end {};

#ifdef DEBUG
	std::size_t i {};
#endif

	while (start != end) {
		crc = ((crc >> 8) & 0xFF) ^ crc16Table[(crc ^ *start++) & 0xFF];
#ifdef DEBUG
		++i;
#endif
	}

#ifdef DEBUG
	std::cout << __func__ << "\tcounter: " << std::dec << i << std::endl;
#endif
	return crc;
}

uint16_t
crc::calculate_crc16(std::istreambuf_iterator<char> start, std::size_t repeat_count)
{
	uint16_t crc {};
	std::size_t i {};
	std::istreambuf_iterator<char> end {};

	while (start != end && i < repeat_count) {
		crc = ((crc >> 8) & 0xFF) ^ crc16Table[(crc ^ *start++) & 0xFF];
		++i;
	}

	if (i != repeat_count)
		std::cerr << __func__ << " error, \texpected count: " << repeat_count << "\tactual count: " << i << std::endl;

#ifdef DEBUG
	std::cout << __func__ << "\tcounter: " << std::dec << i << std::endl;
#endif
	return crc;
}
