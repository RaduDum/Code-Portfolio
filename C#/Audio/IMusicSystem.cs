
using Assets.Script.Core.AssetLoading.Data.Audio.Music;

namespace Assets.Script.Core.Audio
{

    /// <summary>
    /// Dynamic music system permits only one dynamic audio track at a time
    /// </summary>
    public interface IMusicSystem
    {
        void Update();

        void AssignTrackData(TrackData mainCombatTrack);

        /// <summary>
        /// starts the track 
        /// </summary>
        /// <param name="startTimeInFrames"></param>
        void StartLoadedTrack(int startTimeInFrames = 0);

        void SetTrackIntensity(int intensity);

        void PauseLoadedTrack();

        void StopLoadedTrack();

        void ResumeLoadedTrack();

        void RestartLoadedTrack();

        void SetVolume(int volume);

        /// <summary>
        /// in case we want only non-dynamic audio tracks, set this to false
        /// move this to another more general settings class?
        /// </summary>
        void SetDynamicTracksActive(bool flag);

        InitialMusicSelection GetInitialData();

        MusicData GetMusicData();

        DynamicMusicData GetDynamicMusicData();

        bool GetDynamicTracksActive();
    }
}