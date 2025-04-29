// FirmwareUpdates.sol: Solidity Smart Contract for firmware update events
// This contract records firmware update commands on a blockchain (Ethereum).
// It stores the current sensor mode (e.g., Temperature or Humidity) and emits an event on updates.

pragma solidity ^0.8.0;

contract FirmwareUpdates {
    // Only the contract owner (deployer) can issue firmware update commands
    address public owner;
    
    // Enum to represent firmware mode (which sensor to read)
    enum Mode { Temperature, Humidity }
    Mode public currentMode;
    
    // Event to log firmware update commands
    event FirmwareUpdate(address indexed updater, Mode newMode, uint256 timestamp);
    
    constructor() {
        owner = msg.sender;
        currentMode = Mode.Temperature;  // initial firmware mode: Temperature sensor
    }
    
    // Function to update the firmware mode (e.g., switch sensor). Only owner can call.
    function updateMode(Mode newMode) public {
        require(msg.sender == owner, "Only owner can update firmware mode");
        currentMode = newMode;
        emit FirmwareUpdate(msg.sender, newMode, block.timestamp);
    }
}
