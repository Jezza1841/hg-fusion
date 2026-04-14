#include "../include/types.h"
#include "../include/config.h"
#include "../include/debug.h"
#include "../include/sound.h"
#include "../include/NWAVPlayer.h"



int firstWavID = 2088; //defo need to update this



void LONG_CALL NNS_SndInit_Hook(void){
    NNS_SndInit_Original();
    //NWAVPlayer::init();
    //debug_printf("[NNS_SndInit_Hook] Initializing sound system.\n");
}

void LONG_CALL NNS_SndMain_Hook(void){
    NNS_SndMain_Original();
    //NWAVPlayer::updateFade();
    //Works, spams prints to console :D
}

void LONG_CALL NNS_SndPlayerSetTempoRatio_Hook(int handle, int tempo){
    NNS_SndPlayerSetTempoRatio_Original(handle, tempo);
    //debug_printf("[NNS_SndPlayerSetTempoRatio_Hook] Setting tempo ratio to %d.\n", tempo);
    //this still needs to be tested.
    //NWAVPlayer_setSpeed(tempo << 12 >> 8);
}



void LONG_CALL GF_SndHandleMoveVolume_Hook(int param1, int volume, int frames)
{
    GF_SndHandleMoveVolume_Original(param1, volume, frames);
    //debug_printf("[GF_SndHandleMoveVolume_Hook] Handling move volume with params: %d, %d, %d.\n", param1, volume, frames);
    //param 1 could be the player ID? only update volume for bgm, not cries or sfx
    
    if (param1 == 0)
    {
        //debug_printf("Player is BGM (GF wrapper).\n");
        //NWAVPlayer_setVolume(volume, frames);
    }
}

void LONG_CALL NNS_SndPlayerPauseByPlayerNo_Hook(u8 playerID, BOOL paused)
{
    NNS_SndPlayerPauseByPlayerNo_Original(playerID, paused);
    //debug_printf("[NNS_SndPlayerPauseByPlayerNo_Hook] Setting pause for player %d to %d.\n", playerID, paused);
    //field or bgm
    if(playerID == 1 || playerID == 7){
        //NWAVPlayer_setPaused(paused);
        //debug_printf("Player is either field or bgm.\n");
    } 
    
}

void LONG_CALL NNS_SndPlayerStopSeqByPlayerNo_Hook(u8 playerID, int fadeFrame)
{
    NNS_SndPlayerStopSeqByPlayerNo_Original(playerID, fadeFrame);
    //debug_printf("[NNS_SndPlayerStopSeqByPlayerNo_Hook] Stopping sequence for player %d with fade frame %d.\n", playerID, fadeFrame);
    
}

/*
static BOOL GetIfSequenced(int seqID)
{
    int wavID = firstWavID + seqID; //firstWavID is the index in NWAVPlayer.h
    FSFile file;
    FS_InitFile(&file);
    if (FS_OpenFileFast(&file, GetSoundDataPointer(), wavID))
    {
        int magic;
        int readSize = FS_ReadFile(&file, &magic, 4);
        if(readSize)
        {
            if (magic == NWAV)
            {
                FS_CloseFile(&file);
                return FALSE;
            }
        }
        FS_CloseFile(&file);
    }
    return TRUE;
}
*/


//replace the play function
void LONG_CALL PlayBGM_Hook(u16 seqno)
{

    PlayBGM_Original(seqno);
    debug_printf("[PlayBGM_Original] seq=%d\n", seqno);
    
    /*
    int wavID = firstWavID + seqno;

    int* currSeq = (int*)0x02088B64; //this value is for nsmbds, might need the get seq playing?
    if (GetIfSequenced(seqno)){
        if(*currSeq != seqno){
            NWAVPlayer_stop(0);
        }
        PlayBGM_Original(seqno);
        debug_printf("[PlayBGM_Original] seq=%d\n", seqno);
    }
    else{
        if(*currSeq != seqno){
            NNS_SndPlayerStopSeqByPlayerNo_Hook(0, 0);
        }
        NWAVPlayer_play(wavID);
        *currSeq = seqno;
    }
    */

}



BOOL LONG_CALL GF_Snd_LoadSeq(int seqNo) {
    BOOL ret;
    struct SND_WORK *work;
    if (TRUE)//GetIfSequenced(seqNo) || 
    {
        
        work = GetSoundDataPointer();
        ret = NNS_SndArcLoadSeq(seqNo, work->heap);
        GF_SndHeapGetFreeSize();

#ifdef DEBUG_SOUND_SSEQ_LOADS
        if (!ret)
        {
            u8 buf[200];
            sprintf(buf, "[GF_Snd_LoadSeq] Failed to load song %d.  There are 0x%x bytes left in the sound heap.\n", seqNo, SoundHeapFreeSize);
            debugsyscall(buf);
        }
        else
        {
            u8 buf[200];
            sprintf(buf, "[GF_Snd_LoadSeq] Loaded song %d.  There are 0x%x bytes left in the sound heap.\n", seqNo, SoundHeapFreeSize);
            debugsyscall(buf);
        }
    }
#endif // DEBUG_SOUND_SSEQ_LOADS
    else
    {
        debug_printf("[GF_Snd_LoadSeq] Bypassed heap load for NWAV song %d.\n", seqNo);
        ret = TRUE;
    }
    
    return ret;
}


BOOL GF_Snd_LoadSeqEx(int seqNo, u32 loadFlag) {
    BOOL ret;
    struct SND_WORK *work;

    work = GetSoundDataPointer();
    ret = NNS_SndArcLoadSeqEx(seqNo, loadFlag, work->heap);
    GF_SndHeapGetFreeSize();

#ifdef DEBUG_SOUND_SSEQ_LOADS
    if (!ret)
    {
        u8 buf[200];
        sprintf(buf, "[GF_Snd_LoadSeqEx] Failed to load song %d.  There are 0x%x bytes left in the sound heap.\n", seqNo, SoundHeapFreeSize);
        debugsyscall(buf);
    }
    else
    {
        u8 buf[200];
        sprintf(buf, "[GF_Snd_LoadSeqEx] Loaded song %d.  There are 0x%x bytes left in the sound heap (EX).\n", seqNo, SoundHeapFreeSize);
        debugsyscall(buf);
    }
#endif // DEBUG_SOUND_SSEQ_LOADS

    return ret;
}


#ifdef DEBUG_SOUND_SBNK_LOADS

const u8 *NNS_SND_ARC_LOAD_ERROR_STRINGS[] =
{
    "NNS_SND_ARC_LOAD_SUCCESS",
    "NNS_SND_ARC_LOAD_ERROR_INVALID_GROUP_NO",
    "NNS_SND_ARC_LOAD_ERROR_INVALID_SEQ_NO",
    "NNS_SND_ARC_LOAD_ERROR_INVALID_SEQARC_NO",
    "NNS_SND_ARC_LOAD_ERROR_INVALID_BANK_NO",
    "NNS_SND_ARC_LOAD_ERROR_INVALID_WAVEARC_NO",
    "NNS_SND_ARC_LOAD_ERROR_FAILED_LOAD_SEQ",
    "NNS_SND_ARC_LOAD_ERROR_FAILED_LOAD_SEQARC",
    "NNS_SND_ARC_LOAD_ERROR_FAILED_LOAD_BANK",
    "NNS_SND_ARC_LOAD_ERROR_FAILED_LOAD_WAVE"
};

#endif // DEBUG_SOUND_SBNK_LOADS


int LONG_CALL NNSi_SndArcLoadBank(int bankNo, u32 loadFlag, void *heap, BOOL bSetAddr, struct SNDBankData** pData)
{
    const NNSSndArcBankInfo* bankInfo;
    const NNSSndArcWaveArcInfo* waveArcInfo;
    SNDBankData* bank = NULL;
    SNDWaveArc* waveArc = NULL;
    int result;
    int i;
    BOOL loadingNewCry = 0, hasLoadedCry = 0;

    // Get bank information
    if (bankNo >= CRY_PSEUDOBANK_START || (bankNo < 495 && bankNo > 1)) // assume all cry banks are loading cries
    {
        bankInfo = NNS_SndArcGetBankInfo(1);
        loadingNewCry = 1;
#ifdef DEBUG_SOUND_SBNK_LOADS
        u8 buf[200];
        sprintf(buf, "[NNSi_SndArcLoadBank] Cry load detected for bank %d (Index %d).\n", bankNo, (bankNo >= CRY_PSEUDOBANK_START) ? (bankNo - (CRY_PSEUDOBANK_START - 544)) : bankNo);
        debugsyscall(buf);
#endif // DEBUG_SOUND_SBNK_LOADS
    }
    else
    {
        bankInfo = NNS_SndArcGetBankInfo( bankNo );
    }

#ifdef DEBUG_SOUND_SBNK_LOADS
    if (bankInfo == NULL)
    {
        u8 buf[200];
        GF_SndHeapGetFreeSize();
        sprintf(buf, "[NNSi_SndArcLoadBank] Failed to load bank %d.  There are 0x%x bytes left in the sound heap.\n", bankNo, SoundHeapFreeSize);
        debugsyscall(buf);
    }
#endif // DEBUG_SOUND_SBNK_LOADS

    if ( bankInfo == NULL ) return NNS_SND_ARC_LOAD_ERROR_INVALID_BANK_NO;

    // If necessary to load
    if ( loadFlag & NNS_SND_ARC_LOAD_BANK )
    {
        bank = LoadBank( bankInfo->fileId, heap, bSetAddr );
        if ( bank == NULL ) {
            return NNS_SND_ARC_LOAD_ERROR_FAILED_LOAD_BANK;
        }
    }
    else
    {
        bank = (SNDBankData*)NNS_SndArcGetFileAddress( bankInfo->fileId );
    }

    // Load waveform data
    for( i = 0; i < NNS_SND_ARC_BANK_TO_WAVEARC_NUM ; i++ )
    {
        u32 waveArcIndex = bankInfo->waveArcNo[i];
        if (loadingNewCry && !hasLoadedCry)
        {
            waveArcIndex = bankNo;
            hasLoadedCry = 1;
        }

        if ( waveArcIndex == NNS_SND_ARC_INVALID_WAVEARC_NO ) continue;

        // Get waveform archive information
        waveArcInfo = NNS_SndArcGetWaveArcInfo( waveArcIndex );

        if (waveArcInfo == NULL)
        {
#ifdef DEBUG_SOUND_SBNK_LOADS
            u8 buf[200];
            GF_SndHeapGetFreeSize();
            sprintf(buf, "[NNSi_SndArcLoadBank] Failed to load waveArc %d using NNS_SndArcGetWaveArcInfo.  There are 0x%x bytes left in the sound heap.\n", waveArcIndex, SoundHeapFreeSize);
            debugsyscall(buf);
#endif // DEBUG_SOUND_SBNK_LOADS
            
            return NNS_SND_ARC_LOAD_ERROR_INVALID_WAVEARC_NO;
        }

        // Loading waveform archives
        result = NNSi_SndArcLoadWaveArc( waveArcIndex, loadFlag, heap, bSetAddr, &waveArc );

#ifdef DEBUG_SOUND_SBNK_LOADS

        if ( result != NNS_SND_ARC_LOAD_SUCCESS )
        {
            u8 buf[200];
            GF_SndHeapGetFreeSize();
            if (loadingNewCry)
            {
                sprintf(buf, "[NNSi_SndArcLoadBank] Failure to load waveArc %d using NNSi_SndArcLoadWaveArc (%s) ignored because cry detected and debugging is on.  There are 0x%x bytes left in the sound heap.\n", waveArcIndex,  NNS_SND_ARC_LOAD_ERROR_STRINGS[result], SoundHeapFreeSize);
                debugsyscall(buf);
            }
            else
            {
                sprintf(buf, "[NNSi_SndArcLoadBank] Failed to load waveArc %d using NNSi_SndArcLoadWaveArc (%s).  There are 0x%x bytes left in the sound heap.\n", waveArcIndex,  NNS_SND_ARC_LOAD_ERROR_STRINGS[result], SoundHeapFreeSize);
                debugsyscall(buf);
                return result;
            }
        }

#else

        if ( result != NNS_SND_ARC_LOAD_SUCCESS ) return result;

#endif // DEBUG_SOUND_SBNK_LOADS

        if ( waveArcInfo->flags & NNS_SND_ARC_WAVEARC_SINGLE_LOAD )
        {
            // Individual waveform loading
            if ( loadFlag & NNS_SND_ARC_LOAD_WAVE )
            {
                if ( ! LoadSingleWaves( waveArc, bank, i, waveArcInfo->fileId, heap ) )
                {
#ifdef DEBUG_SOUND_SBNK_LOADS
                    {
                        u8 buf[200];
                        GF_SndHeapGetFreeSize();
                        sprintf(buf, "[NNSi_SndArcLoadBank] Failed to load waves for waveArc id %d using LoadSingleWaves.  There are 0x%x bytes left in the sound heap.\n", waveArcIndex, SoundHeapFreeSize);
                        debugsyscall(buf);
                    }
#endif // DEBUG_SOUND_SBNK_LOADS

                    return NNS_SND_ARC_LOAD_ERROR_FAILED_LOAD_WAVE;
                }
            }
        }

        // Associate waveforms with banks
        if ( bank != NULL && waveArc != NULL ) {
            SND_AssignWaveArc( bank, i, waveArc );

#ifdef DEBUG_SOUND_SBNK_LOADS
            {
                u8 buf[200];
                GF_SndHeapGetFreeSize();
                sprintf(buf, "[NNSi_SndArcLoadBank] Loaded waveArc id %d fully and assigned it to in-progress loaded bank %d.  There are 0x%x bytes left in the sound heap.\n", waveArcIndex, bankNo, SoundHeapFreeSize);
                debugsyscall(buf);
            }
#endif // DEBUG_SOUND_SBNK_LOADS

        }

    }

    if ( pData != NULL ) *pData = bank;

#ifdef DEBUG_SOUND_SBNK_LOADS
    {
        u8 buf[200];
        GF_SndHeapGetFreeSize();
        sprintf(buf, "[NNSi_SndArcLoadBank] Loaded bank %d.  There are 0x%x bytes left in the sound heap.\n", bankNo, SoundHeapFreeSize);
        debugsyscall(buf);
    }
#endif // DEBUG_SOUND_SBNK_LOADS

    return NNS_SND_ARC_LOAD_SUCCESS;
}

