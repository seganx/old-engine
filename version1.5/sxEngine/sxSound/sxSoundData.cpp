#include "sxSoundData.h"
#include "sxSoundDevice.h"
#include "../sxSystem/sxSystem.h"
#include "bass.h"


#define SOUNDDATA_MAXCHNL	5				//  maximum channels used in samples	
#define SOUNDDATA_PREVIEW	0x0000001		//  sound data is in preview mode

typedef Map<UINT, sx::snd::PSoundData>	sxMapSoundData;

static const char*	SoundTitleFile = "SeganX Sound Data File - sajad.b@gmail.com";

//////////////////////////////////////////////////////////////////////////
//	STATIC VARIABLES
//////////////////////////////////////////////////////////////////////////
static sxMapSoundData	s_mapSoundData;



// derived sound data class is the main sound data class
class DerivedSoundData : public sx::snd::SoundData
{
	SEGAN_STERILE_CLASS(DerivedSoundData);

public:
	DerivedSoundData(void): m_Option(0), m_Handle(0), m_RefCount(1)
	{
		m_ID = sx::cmn::ID_Generate(60);
	}

	virtual ~DerivedSoundData(void)
	{
		CleanUp();
	}

	FORCEINLINE DWORD GetID( void )
	{
		return m_ID;
	}

	void SetSource(const WCHAR* srcAddress)
	{
		if ( m_Option & SOUNDDATA_PREVIEW ) return;

		UINT id = sx::cmn::GetCRC32(srcAddress);

		sx::snd::PSoundData sd = NULL;
		if (s_mapSoundData.Find(m_ID, sd) && sd==this)
		{
			s_mapSoundData.Remove(m_ID);
			s_mapSoundData.Insert(id, this);
		}

		m_Src = srcAddress;
		m_ID = id;

		//  load data from source file
		LoadData( NULL );
	}

	const WCHAR* GetSource(void)
	{
		return m_Src;
	}

	const PSoundDataDesc GetDesc(void)
	{
		return &m_Desc;
	}

	const DWORD GetHandle( void )
	{
		if ( m_Desc.type == ST_SAMPLE )
			return BASS_SampleGetChannel( m_Handle, FALSE );
		else
			return m_Handle;
	}

	void SetDesc(SoundDataDesc& desc)
	{
		if ( m_Option & SOUNDDATA_PREVIEW ) return;

// 		//  update description
// 		if ( m_Desc.type == ST_SAMPLE )
// 		{
//  			BASS_SAMPLE info;
//  			BASS_SampleGetInfo( m_Handle, &info );
// 
// 			if ( desc.freq )
// 				m_Desc.freq = desc.freq;
// 
//  			BASS_SampleSetInfo( m_Handle, &info);
// 		}
// 		else if ( desc.type == ST_STREAM )
// 		{
// 		}

		//  apply changes
		if ( desc.is3D != m_Desc.is3D )
		{
			m_Desc.is3D = desc.is3D;
			LoadData( &m_Desc );
		}
	}

	bool LoadFromFile( const WCHAR* fileName, SoundDataDesc& newDesc, bool Import /*= false */ )
	{
		if ( !sx::snd::Device::GetInfo()->inited || !fileName ) return false;

		WCHAR* cFileName = (WCHAR*)fileName;
		char filename[2048]; ZeroMemory(filename, 2048);
		int filenameLen = 0;
		while ( *cFileName )
			filename[ filenameLen++ ] = (char)*(cFileName++);		

		//  clear current content
		CleanUp();

		//  prepare flags to load file
		DWORD flags = 0;
		if ( newDesc.is3D )
			flags = BASS_SAMPLE_3D | BASS_SAMPLE_MONO;

		if ( sx::snd::Device::GetInfo()->initFlag & SX_SND_SOFTWARE )
			flags |= BASS_SAMPLE_SOFTWARE | BASS_SAMPLE_MUTEMAX;

		//  load file depend on flags
		if ( newDesc.type == ST_SAMPLE )
		{
			m_Handle = BASS_SampleLoad( FALSE, filename, 0, 0, SOUNDDATA_MAXCHNL, flags );

			if ( m_Handle )
			{
				BASS_SAMPLE info; ZeroMemory(&info, sizeof(info));
				BASS_SampleGetInfo( m_Handle, &info );

				newDesc.lenght		= (float)BASS_ChannelBytes2Seconds( m_Handle, info.length );
				newDesc.channels	= info.chans;
				newDesc.freq		= info.freq;
				newDesc.minDistance = info.mindist;
				newDesc.maxDistance = info.maxdist;
				newDesc.is3D		= SEGAN_SET_HAS( info.flags, BASS_SAMPLE_3D );
			}
		}
		else if ( newDesc.type == ST_STREAM )
		{
			m_Handle = BASS_StreamCreateFile( FALSE, filename, 0, 0, flags | BASS_STREAM_PRESCAN );

			if ( m_Handle )
			{
				BASS_CHANNELINFO info;  ZeroMemory(&info, sizeof(info));
				BASS_ChannelGetInfo( m_Handle, &info );

				QWORD len			= BASS_ChannelGetLength( m_Handle, BASS_POS_BYTE);
				newDesc.lenght		= (float)BASS_ChannelBytes2Seconds( m_Handle, len );
				newDesc.channels	= info.chans;
				newDesc.freq		= info.freq;
				newDesc.minDistance = 0;
				newDesc.maxDistance = 0;
				newDesc.is3D		= SEGAN_SET_HAS( info.flags, BASS_SAMPLE_3D );
			}
		}

		//  handle exceptions
		if ( !m_Handle )
		{
			switch ( BASS_ErrorGetCode() )
			{
			case BASS_ERROR_FILEOPEN:	sxLog::Log(L"Loading sound [%s] failed! The file could not be opened.", fileName);	break;
			case BASS_ERROR_FORMAT:
			case BASS_ERROR_FILEFORM:	sxLog::Log(L"Loading sound [%s] failed! The file's format is not recognized/supported.", fileName);	break;
			case BASS_ERROR_MEM:		sxLog::Log(L"Loading sound [%s] failed! There is not enough memory to load sound data.", fileName);	break;
			case BASS_ERROR_NO3D:		sxLog::Log(L"Loading sound [%s] failed! 3D sound most be MONO!", fileName);	break;
			default:					sxLog::Log(L"Loading sound [%s] failed du to mystery problem!", fileName);	break;
			}

			return false;
		}
		
		//  turn on preview mode
		SEGAN_SET_ADD(m_Option, SOUNDDATA_PREVIEW);

		//  sound has been loaded. prepare to import the sound
		if ( Import )
		{
			if ( !sx::sys::FileManager::Project_GetDir() )
			{
				sxLog::Log(L"Importing sound [%s] failed! project directory not found!", fileName);
				return false;
			}

			//  import the sound to library
			sx::sys::FileStream srcFile;
			if ( srcFile.Open( fileName, FM_OPEN_READ | FM_SHARE_READ ) )
			{
				MemoryStream destFile;

				//  write title
				destFile.Write(SoundTitleFile, (int)strlen(SoundTitleFile));

				//  write file version
				int ver = 1;
				SEGAN_STREAM_WRITE( destFile, ver );

				//  write description
				SEGAN_STREAM_WRITE( destFile, newDesc );

				//	write source file size
				UINT fileSize = srcFile.Size();
				SEGAN_STREAM_WRITE(destFile, fileSize);

				//	write source file
				destFile.CopyFrom( srcFile );

				//  save in the library
				str256 str = fileName; str.ExtractFileName( );
				str.ExcludeFileExtension();	str << L".snd";
				if ( sx::sys::FileManager::File_Save( str, SEGAN_PACKAGENAME_MEIDA, destFile ) )
				{
					m_Src = str;

					//  set new descriptions
					m_Desc = newDesc;

					//  turn off preview mode
					SEGAN_SET_REM(m_Option, SOUNDDATA_PREVIEW);

					sxLog::Log(L"Sound [%s] successfully imported to library.", fileName);
				}

				srcFile.Close();
			}
			else
			{
				sxLog::Log(L"Importing sound [%s] failed! can't open the file!", fileName);
				return false;
			}
		}
		else
		{
			sxLog::Log(L"Loading sound [%s] completed successfully.", fileName);
		}

		return true;
	}

	void Load( Stream& stm, PSoundDataDesc pNewDesc /*= NULL*/ )
	{
		if ( !sx::snd::Device::GetInfo()->inited ) return;

		//  clear current content
		CleanUp();

		//  read title
		char ch[128];
		stm.Read(ch, (int)strlen(SoundTitleFile));

		//  read file version
		int ver = 0;
		SEGAN_STREAM_READ( stm, ver );
		if ( ver == 1 )
		{
			//  read description
			SEGAN_STREAM_READ( stm, m_Desc );

			// check new description
			if ( pNewDesc )
				m_Desc.is3D = pNewDesc->is3D;

			//	read file size
			UINT fileSize = 0;
			SEGAN_STREAM_READ( stm , fileSize );

			//  prepare flags to load file
			DWORD flags = 0;
			if ( m_Desc.is3D )
				flags = BASS_SAMPLE_3D | BASS_SAMPLE_MONO;

			if ( sx::snd::Device::GetInfo()->initFlag & SX_SND_SOFTWARE )
				flags |= BASS_SAMPLE_SOFTWARE | BASS_SAMPLE_MUTEMAX;

			//  load file depend on flags
			if ( m_Desc.type == ST_SAMPLE )
			{
				PBYTE buffer = (PBYTE)sx_mem_alloc(fileSize);
				stm.Read(buffer, fileSize);
				m_Handle = BASS_SampleLoad( TRUE, buffer, 0, fileSize, SOUNDDATA_MAXCHNL, flags | BASS_SAMPLE_OVER_POS );
				sx_mem_free( buffer );
			}
			else if ( m_Desc.type == ST_STREAM )
			{
				// make a temp file to load stream
				m_StreamFile.Format( L"%stmp%d.tmp", sx::sys::GetTempFolder(), (int)sx::cmn::Random(999999) );

				sx::sys::FileStream tempFile;
				if ( tempFile.Open( m_StreamFile, FM_CREATE ) )
				{
					tempFile.CopyFrom( stm, fileSize );
					tempFile.Close();

					//  ah shit! convert from WCHAR to char
					char filename[2048]; ZeroMemory(filename, 2048);
					int filenameLen = 0;
					WCHAR* cFileName = (WCHAR*)m_StreamFile.Text();
					while ( *cFileName )
						filename[ filenameLen++ ] = (char)*(cFileName++);		

					m_Handle = BASS_StreamCreateFile( FALSE, filename, 0, 0, flags );
				}
			}

			//  handle exceptions
			if ( !m_Handle )
			{
				switch ( BASS_ErrorGetCode() )
				{
				case BASS_ERROR_FILEOPEN:	sxLog::Log(L"Loading sound [%s] failed! The file could not be opened.", *m_Src);	break;
				case BASS_ERROR_FORMAT:
				case BASS_ERROR_FILEFORM:	sxLog::Log(L"Loading sound [%s] failed! The file's format is not recognized/supported.", *m_Src);	break;
				case BASS_ERROR_MEM:		sxLog::Log(L"Loading sound [%s] failed! There is not enough memory to load sound data.", *m_Src);	break;
				case BASS_ERROR_NO3D:		sxLog::Log(L"Loading sound [%s] failed! 3D sound most be MONO!", *m_Src);	break;
				default:					sxLog::Log(L"Loading sound [%s] failed du to mystery problem!", *m_Src);	break;
				}
			}

		}  //  if ( ver == 1 )

	}

	void CleanUp( void )
	{
		if ( m_Handle )
		{
			if ( m_Desc.type == ST_SAMPLE )
			{
				BASS_SampleFree( m_Handle );
			}
			else if ( m_Desc.type == ST_STREAM )
			{
				BASS_StreamFree( m_Handle );

				if ( m_StreamFile.Text() )
				{
					sx::sys::RemoveFile( m_StreamFile );
					m_StreamFile.Clear();
				}
			}

			m_Handle = 0;
		}

		m_Option = 0;
	}

	void LoadData( PSoundDataDesc pNewDesc )
	{
		CleanUp();

		PStream pFile = NULL;
		if ( sx::sys::FileManager::File_Open( m_Src, SEGAN_PACKAGENAME_MEIDA, pFile ))
		{
			if ( pNewDesc )
			{
				SoundDataDesc desc = *pNewDesc;
				Load( *pFile, &desc );
			}
			else Load( *pFile, pNewDesc );

			sx::sys::FileManager::File_Close( pFile );
		}
	}

public:
	String				m_Src;				//  Address of resource file
	DWORD						m_ID;				//  I just feel that it will be useful for fast search :)
	DWORD						m_Option;			//  I just feel that it will be useful :)

	DWORD						m_Handle;			//	handle of the sound data in API
	SoundDataDesc				m_Desc;				//	description of the sound file
	String				m_StreamFile;		//  name of stream file to delete on cleanup

	int							m_RefCount;			//  number of references to objects
};



namespace sx { namespace snd {


	//////////////////////////////////////////////////////////////////////////
	//  MANAGER
	//////////////////////////////////////////////////////////////////////////
	bool SoundData::Manager::Exist( OUT PSoundData& pSndData, const WCHAR* src )
	{
		UINT key = sx::cmn::GetCRC32(src);
		return s_mapSoundData.Find(key, pSndData);
	}

	bool SoundData::Manager::Get( OUT PSoundData& pSndData, const WCHAR* src )
	{
		if (!src) 
		{
			pSndData = NULL;
			return false;
		}

		UINT key = sx::cmn::GetCRC32(src);

		if (s_mapSoundData.Find(key, pSndData))
		{
			DerivedSoundData* s = (DerivedSoundData*)pSndData;
			s->m_RefCount++;
		}
		else
		{
			if (Create(pSndData, src))
			{
				s_mapSoundData.Insert(key, pSndData);
			}
			else return false;
		}

		return true;	
	}

	bool SoundData::Manager::Create( OUT PSoundData& pSndData, const WCHAR* src )
	{
		pSndData = sx_new( DerivedSoundData );
		if (pSndData)
		{
			pSndData->SetSource(src);
			return true;
		}
		else return false;
	}

	void SoundData::Manager::Release( PSoundData& pSndData )
	{
		if (!pSndData) return;
		DerivedSoundData* s = (DerivedSoundData*)pSndData;
		pSndData = NULL;

		s->m_RefCount--;
		if (s->m_RefCount>0) return;

		//  remove from texture map
		PSoundData sn = NULL;
		if (s_mapSoundData.Find(s->m_ID, sn) && sn==s)
			s_mapSoundData.Remove(s->m_ID);

		sx_delete_and_null(s);
	}

	void SoundData::Manager::ClearAll( void )
	{
		if ( s_mapSoundData.IsEmpty() ) return;

		String str = L"Releasing remains Sound(s) :\r\n\t\t\t";

		for (sxMapSoundData::Iterator it = s_mapSoundData.First(); !it.IsLast(); it++)
		{
			DerivedSoundData* s = (DerivedSoundData*)(*it);
			str << (s->GetSource()!=NULL ? s->GetSource() : L" ??? no name ???") << L"\r\n\t\t\t";

			sx_delete_and_null(s);
		}
		s_mapSoundData.Clear();

		sxLog::Log(str);	
	}


} }	//	namespace sx { namespace snd {