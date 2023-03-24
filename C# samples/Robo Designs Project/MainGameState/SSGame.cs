using System;
using System.IO;
using Unity.Collections;
using SharedGame;
using Assets.Script.Core.Inputs.Detection;
using Assets.Script.Core.Inputs.Interpretation;
using Assets.Script.Core.Inputs.Interpretation.Implementation;
using Assets.Script.Core.Player;
using Assets.Script.Core.Stage;
using Assets.Script.Core.AssetLoading.Data.Audio.Music;
using Assets.Script.Core.Logging;
using Assets.Script.Core.Networking;
using Assets.Script.Core.Gameplay.GameMode.Data;
using Assets.Script.Core.Gameplay.GameMode;

namespace Assets.Script.Core.Sys
{
    [Serializable]
    public class SSGame : IGame
    {
        public ExternalQueriedSystems Dependencies { get; private set; }

        public IGameMode GameMode => _gameMode;

        public int FrameNumber { get; private set; }

        public int Checksum => GetHashCode();

        public bool IsInitialised => _initialised;

        // external systems - do these need to be external? yes, for direct menu pinging
        private IInputSystem _inputSystem;
        private IParsedInputStorage _inputStorage;

        private IGameMode _gameMode;

        private ISerializationSystem _serializationSystem = new OnlineSerializationSystem();

        // input interpretation is not a thing on menus
        // however, certain ui elements speak w/ the interpreter directly (why?)
        private IInputInterpreter _inputInterpreter;

        private bool _initialised;

        #region public
        public SSGame(IInputSystem inputSystem, IParsedInputStorage inputStorage)
        {
            FrameNumber = 0;

            _inputSystem = inputSystem;
            _inputStorage = inputStorage;

            _inputInterpreter = new InputInterpreter(_inputStorage);

            // pass em to dependencies
            Dependencies = new ExternalQueriedSystems
            {
                RawInputStorage = _inputStorage,
                InputInterpreter = _inputInterpreter
            };
        }


        public void Initialise(CharacterId characterOne, CharacterId characterTwo)
        {
            _gameMode = new VersusGameMode(_serializationSystem, _inputInterpreter);
            _gameMode.Initialise(characterOne, characterTwo, StageId.Salar, CombatTrackId.RedTheme);

            _initialised = true;
        }


        public void Initialise(GameStateInitialisationData initialisationData)
        {
            _gameMode = new VersusGameMode(_serializationSystem, _inputInterpreter);
            _gameMode.Initialise(
                initialisationData.PlayerData[PlayerId.PlayerOne].SelectedCharacterId,
                initialisationData.PlayerData[PlayerId.PlayerTwo].SelectedCharacterId,
                initialisationData.Stage.SelectedStageId
                , CombatTrackId.RedTheme);

            _initialised = true;
        }

        // serialize local game state
        public void Serialize(BinaryWriter bw)
        {
            bw.Write(FrameNumber);

            Log.Print(LogLevel.Debug1, LogChannels.GGPO, $"[Serialize] {FrameNumber}");
            _serializationSystem.SerializeAll(bw);

        }

        // de-serialize old local game state and try to re-sync
        public void Deserialize(BinaryReader br)
        {
            Log.Print(LogLevel.Error, LogChannels.GGPO, $"[Deserialize] START - {FrameNumber}");

            FrameNumber = br.ReadInt32();
            Log.SetGGPOCurrentFrame(FrameNumber);

            _serializationSystem.DeserializeAll(br);
        }

        public void FreeBytes(NativeArray<byte> data)
        {
            if (data.IsCreated)
            {
                data.Dispose();
            }
        }

        public void FromBytes(NativeArray<byte> bytes)
        {
            using (var memoryStream = new MemoryStream(bytes.ToArray()))
            {
                using (var reader = new BinaryReader(memoryStream))
                {
                    Deserialize(reader);
                }
            }
        }

        public void LogInfo(string filename)
        {
            //TODO: implement me later
        }

        public long ReadInputs(int controllerId)
        {
            var inputs = _inputSystem.GetRawInputsForAllPlayers();
            return inputs[controllerId];
        }

        public NativeArray<byte> ToBytes()
        {
            using (var memoryStream = new MemoryStream())
            {
                using (var writer = new BinaryWriter(memoryStream))
                {
                    Serialize(writer);
                }
                return new NativeArray<byte>(memoryStream.ToArray(), Allocator.Persistent);
            }
        }

        public void Update(long[] inputs, int disconnectFlags)
        {
            if (!_initialised)
            {
                return;
            }

            FrameNumber++;
            Log.SetGGPOCurrentFrame(FrameNumber);

            for (int i = 0; i < inputs.Length; i++)
            {
                //if (inputs[i] != 11111111111)
                {
                    Log.Print(LogLevel.Debug1, LogChannels.GGPO, $"[RAWComparison][Update] Received inputs for controllerID: {i} " +
                        $"- resulting input is {inputs[i]}");
                }
            }

            _inputStorage.Update(FrameNumber);

            _inputStorage.SetInputContext(_inputSystem.GetInputContext());

            // assign raw input
            _inputStorage.AssignRawInputs(inputs);

            // read the inputs and interpret them
            _inputInterpreter.Update(FrameNumber);

            _gameMode.Update(FrameNumber);
        }



        public override int GetHashCode()
        {
            int hashCode = -1214587014;
            hashCode = hashCode * -1521134295 + FrameNumber.GetHashCode();

            // TODO: get entity hashcode

            return hashCode;
        }

        #endregion
    }
}