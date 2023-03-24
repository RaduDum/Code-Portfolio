using Assets.Script.Core.Logging;
using System;
using System.Collections.Generic;

namespace Assets.Script.Libraries.FiniteStateMachine
{
    public class State<TState, TTrigger>
        where TState : IComparable, IFormattable, IConvertible
        where TTrigger : IComparable, IFormattable, IConvertible
    {
        // current state enum Id

        // key is trigger enum Id, value is next state enum Id to connect to
        private Dictionary<TTrigger, TState> _connections = new Dictionary<TTrigger, TState>();

        internal bool Entered => _entered;
        internal bool Exited => _exited;

        private bool _entered;
        private bool _exited;

        private Action _onEntry;
        private Func<TTrigger> _onUpdate;
        private Action _onExit;

        #region Public Functions

        internal void Entry(bool executeInvoking = true)
        {
            _entered = true;
            if (executeInvoking)
            {
                _onEntry?.Invoke();
            }
        }

        internal bool Update(out TState result)
        {
            if (_onUpdate != default)
            {
                var trigger = _onUpdate.Invoke();

                if (GetConnectionForTrigger(trigger, out result))
                {
                    return true;
                }
            }

            result = default;
            return false;
        }

        internal bool GetConnectionForTrigger(TTrigger trigger, out TState result)
        {
            if (_connections.ContainsKey(trigger))
            {
                result = _connections[trigger];
                return true;
            }
            else
            {
                result = default;
                return false;
            }
        }

        internal void Exit()
        {
            _exited = true;
            _onExit?.Invoke();
        }

        /// <summary>
        /// Reset exit and entered flags
        /// </summary>
        internal void ResetFlags()
        {
            _exited = false;
            _entered = false;
        }

        #endregion

        #region Builder Functions

        /// <summary>
        /// Add a state transition from current state
        /// </summary>
        /// <param name="trigger">Listenable trigger enum that cause the state to progress on activation.</param>
        /// <param name="destination">State enum for where this </param>
        /// <returns>This builder object</returns>
        public State<TState, TTrigger> OnTrigger(TTrigger trigger, TState destination)
        {
            if (trigger.GetType() == destination.GetType())
            {
                Log.Print(LogLevel.Error, LogChannels.FSM, "Trigger and Destination are of the same type. Please feed in correct enum types.");
                return this;
            }

            _connections.Add(trigger, destination);
            return this;
        }

        /// <summary>
        /// Set the action that will run when entering this state
        /// </summary>
        /// <param name="onEntry">Action that runs on entry</param>
        /// <returns>This builder object</returns>
        public State<TState, TTrigger> OnEntry(Action onEntry)
        {
            _onEntry = onEntry;
            return this;
        }

        /// <summary>
        /// Set the action that will run when updating this state
        /// </summary>
        /// <param name="onUpdate">Action that runs on update - should return the same type of trigger type enum that has been added via OnTrigger</param>
        /// <returns>This builder object</returns>
        public State<TState, TTrigger> OnUpdate(Func<TTrigger> onUpdate)
        {
            _onUpdate = onUpdate;
            return this;
        }

        /// <summary>
        /// Set the action that will run when exiting this state
        /// </summary>
        /// <param name="onExit">Action that runs on exit</param>
        /// <returns>This builder object</returns>
        public State<TState, TTrigger> OnExit(Action onExit)
        {
            _onExit = onExit;
            return this;
        }

        #endregion
    }
}