
using Assets.Script.Core.AssetLoading.Data.Audio.Music;
using System;

namespace Assets.Script.Core.Audio
{

    public class MusicSystem : IMusicSystem
    {
        private int _intensity;

        private bool _dynamicTracksEnabled = true;
        private bool _isPlaying;
        private bool _isPaused;

        private int _trackCurrentFrame = 0;

        private InitialMusicSelection _initialTrackData;
        private MusicData _musicTrackData;
        private DynamicMusicData _dynamicMusicData;
        private int _volume = 100;

        public void AssignTrackData(TrackData mainCombatTrack)
        {
            _initialTrackData.IsDynamicMusicEnabled = _dynamicTracksEnabled;
            if (_dynamicTracksEnabled)
            {
                _initialTrackData.MusicTrack = mainCombatTrack.Dynamic;
            }
            else
            {
                _initialTrackData.MusicTrack = mainCombatTrack.Normal;
            }

        }


        public void Update()
        {
            UpdateMusicData();
            UpdateDynamicMusicData();

            if (_isPlaying && !_isPaused)
            {
                _trackCurrentFrame++;
            }

        }

        private void UpdateMusicData()
        {
            _musicTrackData.IsPaused = _isPaused;
            _musicTrackData.IsPlaying = _isPlaying;
            _musicTrackData.StartTime = _trackCurrentFrame;
            _musicTrackData.Volume = _volume;
        }

        private void UpdateDynamicMusicData()
        {
            if (_dynamicTracksEnabled)
            {
                _dynamicMusicData.CurrentLevel = _intensity;
            }
            else
            {
                _dynamicMusicData.CurrentLevel = 0;
            }
        }

        public void StartLoadedTrack(int startTimeInFrames = 0)
        {
            _isPlaying = true;
            _isPaused = false;
            _trackCurrentFrame = startTimeInFrames;
        }

        public void PauseLoadedTrack()
        {
            _isPaused = true;
        }

        public void RestartLoadedTrack()
        {
            _trackCurrentFrame = 0;
        }

        public void ResumeLoadedTrack()
        {
            _isPaused = false;
        }


        public void StopLoadedTrack()
        {
            _isPlaying = false;
            _isPaused = false;
            _trackCurrentFrame = 0;
        }

        public void SetDynamicTracksActive(bool flag)
        {
            _dynamicTracksEnabled = flag;
        }

        public void SetTrackIntensity(int intensity)
        {
            _intensity = Math.Clamp(intensity, 0, 5);
        }

        public InitialMusicSelection GetInitialData()
        {
            return _initialTrackData;
        }

        public MusicData GetMusicData()
        {
            return _musicTrackData;
        }

        public DynamicMusicData GetDynamicMusicData()
        {
            return _dynamicMusicData;
        }

        public void SetVolume(int volume)
        {
            _volume = volume;
        }

        public bool GetDynamicTracksActive()
        {
            return _dynamicTracksEnabled;
        }
    }
}