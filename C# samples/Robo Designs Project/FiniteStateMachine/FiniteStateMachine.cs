using System;
using System.Collections.Generic;
using System.IO;

namespace Assets.Script.Libraries.FiniteStateMachine
{
    public class FiniteStateMachine<TState, TTrigger>
        where TState : IComparable, IFormattable, IConvertible
        where TTrigger : IComparable, IFormattable, IConvertible
    {
        private Dictionary<TState, State<TState, TTrigger>> _states = new Dictionary<TState, State<TState, TTrigger>>();
        private State<TState, TTrigger> _currentState;


        private TState _stateId;
        private TState _lastState;
        private TState _secondToLastState;

        private bool _initialised;

#if DEBUG

        private string _stateIdName = "";
        private string _lastStateName = "";
        private string _secondToLastStateName = "";

        private Dictionary<TState, string> _stateNames = new Dictionary<TState, string>();

        public string SecondLastStateName => _secondToLastStateName;
        public string LastStateName => _lastStateName;
        public string StateName => _stateIdName;

#endif

        public TState SecondLastState => _secondToLastState != null ? _secondToLastState : default;
        public TState LastState => _lastState != null ? _lastState : default;
        public TState State => _stateId != null ? _stateId : default;

        // TODO: could probably optimize by asking the state directly if its been entered
        // but this seems pretty painless to do instead
        // hacky in any case, ideally fixed but not urgent
        private bool _enteredNewStateThisFrame = false;


        public State<TState, TTrigger> AddState(TState state)
        {
            if (!_initialised)
            {
                _stateId = state;
                _initialised = true;
            }

            var result = new State<TState, TTrigger>();
            _states.Add(state, result);

#if DEBUG
            _stateNames.Add(state, state.ToString());
#endif

            return result;
        }

        public void Update()
        {
            if (_currentState == default)
            {
                _currentState = _states[_stateId];
            }

            if (_currentState.Entered == false)
            {
                _currentState.Entry();
                _enteredNewStateThisFrame = true;
            }
            else
            {
                _enteredNewStateThisFrame = false;
            }

            if (_currentState.Update(out var destinationStateId))
            {
                TryTransitionToState(destinationStateId);
            }
        }

        public void ForceTrigger(TTrigger trigger)
        {
            if (_currentState != default)
            {
                if (_currentState.GetConnectionForTrigger(trigger, out var result))
                {
                    TryTransitionToState(result);
                }
            }
        }

        private void TryTransitionToState(TState destinationState, bool executeEnter = true)
        {
            _secondToLastState = _lastState;
            _lastState = _stateId;

            _stateId = destinationState;

#if DEBUG
            if (_stateNames.ContainsKey(_stateId))
            {
                _stateIdName = _stateNames[_stateId];
            }

            if (_stateNames.ContainsKey(_lastState))
            {
                _lastStateName = _stateNames[_lastState];
            }

            if (_stateNames.ContainsKey(_secondToLastState))
            {
                _secondToLastStateName = _stateNames[_secondToLastState];
            }
#endif

            if (executeEnter)
            {
                _currentState.Exit();
            }
            _currentState.ResetFlags();
            _currentState = default;

            // just enter the new one right away, to handle any pre-frame requests
            _currentState = _states[_stateId];
            _currentState.Entry(executeEnter);
        }

        public void Serialize(BinaryWriter bw)
        {
            bw.Write(_enteredNewStateThisFrame);
#if DEBUG
            bw.Write(_stateIdName);
            bw.Write(_lastStateName);
            bw.Write(_secondToLastStateName);
#endif
        }

        public void Deserialize(BinaryReader bw)
        {
            _enteredNewStateThisFrame = bw.ReadBoolean();
#if DEBUG
            _stateIdName = bw.ReadString();
            _lastStateName = bw.ReadString();
            _secondToLastStateName = bw.ReadString();
#endif
        }

        public void ForceState(TState currentState)
        {
            TryTransitionToState(currentState, _enteredNewStateThisFrame);
        }
    }
}