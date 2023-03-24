using UnityEngine;
using UnityEngine.UI;
using System;
using System.Collections.Generic;
using System.Linq;
using Assets.Script.Core.Logging;
using Assets.Script.Libraries.FiniteStateMachine;

namespace Assets.Script.Core.Audio
{

    public class DynamicMusicPrototype : MonoBehaviour
    {
        [System.Serializable]
        public class MusicTrack
        {
            public string Name;

            public AudioClip Clip;
        }

        [System.Serializable]
        public class AudioIntensity
        {
            public int Intensity;

            [SerializeField]
            private MusicTrack _normal;

            [SerializeField]
            private MusicTrack _alternate;

            [SerializeField]
            private MusicTrack _drop;

            public MusicTrack GetCurrent(bool requestAlternate, bool requestDrop)
            {
                Log.Print(LogLevel.Debug2, LogChannels.Music, "[GetCurrent] Requesting track with intensity {Intensity} - alternate: {requestAlternate}, drop: {requestDrop}");

                if (requestDrop)
                {
                    if (_drop.Clip != default)
                    {
                        return _drop;
                    }
                }

                if (requestAlternate)
                {
                    if (_alternate.Clip != default)
                    {
                        return _alternate;
                    }
                }

                return _normal;
            }

        }

        private const int MINIMUM_INTENSITY = 1;
        private const int MAXIMUM_INTENSITY = 6;

        private Scrollbar _scrollbar;

        [SerializeField]
        private Text _internalIntensityPrint;

        [SerializeField]
        private Text _currentIntensityPrint;

        [SerializeField]
        private Text _extraInfo;

        [SerializeField]
        private Text _verboseInfo;


        [SerializeField]
        private List<AudioIntensity> _intensities = new List<AudioIntensity>();

        [SerializeField]
        private MusicTrack _introTrack;

        [SerializeField]
        private MusicTrack _bridge;

        [SerializeField]
        private Button _interrupt;

        private MusicTrack _currentTrack;
        private AudioIntensity _currentAudioIntensity;

        private AudioSource _source;

        private int _previousIntensity;

        private int _currentIntensity = 2;
        private float _lerpingIntensity = 2.0f;
        private float _destinationIntensity;

        private float _currentTime;

        private const float INTENSITY_LIMIT = 0.1f;
        private const float LERP_EPSILON = 0.5f;
        private const float TRANSITION_STEP = 10f;

        private const float CLIP_LENGTH_BUFFER = 0.1f;
        private const float CLIP_BRIDGE_EPSILON = 0.5f;

        private bool _alternateSelected = false;

        private FiniteStateMachine<MusicState, MusicTrigger> _fsm;

        private bool _executedInterrupt;
        private int _interruptIntensity;

        private enum MusicState
        {
            RoundStart,
            Dynamic,
            Interrupt,
            RoundEnd,
            MatchEnd
        }


        private enum MusicTrigger
        {
            Continue,
            Interrupt,
            NO_PROGRESS
        }

        private void Awake()
        {
            _source = gameObject.AddComponent<AudioSource>();
            _scrollbar = this.GetComponent<Scrollbar>();

            _fsm = new FiniteStateMachine<MusicState, MusicTrigger>();

            _fsm.AddState(MusicState.RoundStart)
                .OnEntry(OnRoundStartEntry)
                .OnUpdate(OnRoundStartUpdate)
                .OnTrigger(MusicTrigger.Continue, MusicState.Dynamic);

            _fsm.AddState(MusicState.Dynamic)
                .OnUpdate(OnDynamicUpdate)
                .OnTrigger(MusicTrigger.Interrupt, MusicState.Interrupt)
                .OnTrigger(MusicTrigger.Continue, MusicState.RoundEnd);

            _fsm.AddState(MusicState.Interrupt)
                .OnEntry(OnInterruptEntry)
                .OnUpdate(OnInterruptUpdate)
                .OnTrigger(MusicTrigger.Continue, MusicState.Dynamic);
        }

        private void Start()
        {
            _interrupt.onClick.AddListener(() => ExecuteInterrupt());
        }

        // Update is called once per frame
        void Update()
        {
            _fsm.Update();

            UpdateCurrentTrack();

            UpdateCurrentIntensity(Time.deltaTime);
        }


        private void OnRoundStartEntry()
        {
            AssignTrack(_introTrack);
            _verboseInfo.text = "Simulating Round Start...";
        }

        private MusicTrigger OnRoundStartUpdate()
        {
            PrintCurrentTrack();

            // intro runs just once -> switch to 2
            if (_introTrack.Name == _currentTrack.Name)
            {
                if (_currentTrack.Clip.length - _source.time <= CLIP_LENGTH_BUFFER)
                {
                    AssignIntensity(_intensities.First(x => x.Intensity == 2));
                    _verboseInfo.text = "Round Start > initial neutral";
                    return MusicTrigger.Continue;
                }
            }

            return MusicTrigger.NO_PROGRESS;
        }

        private MusicTrigger OnDynamicUpdate()
        {
            PrintCurrentTrack();

            if (_executedInterrupt)
            {
                return MusicTrigger.Interrupt;
            }

            UpdateCurrentTrack();

            return MusicTrigger.NO_PROGRESS;
        }

        private void OnInterruptEntry()
        {
            _executedInterrupt = false;
            _verboseInfo.text = $"Executing interrupt - {_bridge.Name}";

            AssignTrack(_currentTrack, true);
        }


        private MusicTrigger OnInterruptUpdate()
        {
            PrintCurrentTrack();

            if (_currentTrack.Clip.length - _source.time <= CLIP_LENGTH_BUFFER)
            {
                _verboseInfo.text = "Playing interrupt intensity";
                AssignIntensity(_intensities.First(x => x.Intensity == _interruptIntensity));
                return MusicTrigger.Continue;
            }

            return MusicTrigger.NO_PROGRESS;
        }

        private void ExecuteInterrupt()
        {
            _executedInterrupt = true;
            _interruptIntensity = _currentIntensity;
        }

        private void PrintCurrentTrack()
        {
            if (_source.isPlaying)
            {
                _extraInfo.text = $"Current clip {_currentTrack.Name} - at {_source.time}";
            }
        }

        private void UpdateCurrentTrack()
        {
            if (_executedInterrupt)
            {
                return;
            }


            if (_currentTrack != default)
            {
                if (_currentTrack.Clip.length - _source.time <= CLIP_LENGTH_BUFFER)
                {
                    AssignIntensity(_intensities.First(x => x.Intensity == _currentIntensity));
                    _verboseInfo.text = "Reading Slider for intensity";
                }
            }
        }

        private void AssignIntensity(AudioIntensity intensity)
        {
            if (intensity != default)
            {
                var requestAlternate = intensity.Intensity == _currentAudioIntensity?.Intensity && _alternateSelected == false;
                var requestDrop = _previousIntensity > _currentIntensity;

                _previousIntensity = _currentAudioIntensity != default ? _currentAudioIntensity.Intensity : _currentIntensity;
                _alternateSelected = requestAlternate;

                _currentAudioIntensity = intensity;

                AssignTrack(intensity.GetCurrent(requestAlternate, requestDrop));
            }
        }

        private void AssignTrack(MusicTrack track, bool interrupted = false)
        {
            Log.Print(LogLevel.Debug2, LogChannels.Music, $"[AssignTrack] Assigning new track with name - {track.Name}");

            // keep the same track if an invalid one is passed
            if (track == default)
            {
                track = _currentTrack;
            }

            float time = 0.0f;
            if (interrupted)
            {
                time = _currentTrack.Clip.length - CLIP_BRIDGE_EPSILON;
            }

            _currentTrack = track;

            _source.clip = _currentTrack.Clip;
            _source.time = time;
            _source.Play();
        }

        private void UpdateCurrentIntensity(float timeStep)
        {
            _destinationIntensity = GetScrollValue();

            var difference = _destinationIntensity - _lerpingIntensity;
            float step = 0.0f;

            Log.Print(LogLevel.Debug3, LogChannels.Music, $"[UpdateCurrentIntensity] current difference is - {difference}");

            if (Mathf.Abs(difference) > INTENSITY_LIMIT)
            {
                step = (difference * TRANSITION_STEP + LERP_EPSILON) * timeStep;
                _lerpingIntensity += step;
            }
            else
            {
                // store the last intensity
                var currentIntensity = Mathf.RoundToInt(_lerpingIntensity);
                _currentIntensity = currentIntensity;
            }

            Log.Print(LogLevel.Debug3, LogChannels.Music, $"[UpdateCurrentIntensity] current step is - {step}");


            _internalIntensityPrint.text = _lerpingIntensity.ToString();
            _currentIntensityPrint.text = _currentIntensity.ToString();
        }

        private float GetScrollValue()
        {
            var value = 1 + _scrollbar.value * (MAXIMUM_INTENSITY - MINIMUM_INTENSITY);

            return value;
        }

    }
}