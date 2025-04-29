# Security Through Decentralization

Summary
This repository contains all the code that backs the senior-thesis project “Security Through Decentralization: Blockchain’s Role in IoT Networks.” Three alternative firmware-update architectures for a fleet of 10 Arduino MKR WiFi 1010 + MKR ENV Shield sensors were built and benchmarked:

Local Python Server – classic client–server on a LAN

Arduino IoT Cloud – vendor cloud OTA updates

Blockchain-Integrated – private Ethereum PoA ledger + smart-contract-driven updates

The repo also includes Arduino sketches that simulate DDoS and MitM attacks so you can reproduce the security tests.

Path | What it contains | Runs on
IoT Network/ | Three Arduino sketches – one per architecture. • ArduinoSensorNode_Local.ino (posts JSON data to server.py) GitHubGitHub • ArduinoSensorNode_CloudIoT.ino (pushes data to Arduino IoT Cloud) GitHubGitHub • ArduinoSensorNode_Blockchain.ino (polls FirmwareUpdates smart contract and switches sensor mode) GitHubGitHub | Arduino MKR WiFi 1010 + MKR ENV Shield
Hacking Attacks/ | Adversary sketches. • DDoS_Attack.ino – floods a target host/port with HTTP to overwhelm it GitHubGitHub • MitM_Attack.ino – tampers with sensor data before relaying to the server GitHubGitHub | Any spare MKR WiFi 1010
server.py | Minimal Flask API that receives sensor JSON (/sensor-data) and logs it GitHub | Python 3
smartcontracts.sol | Solidity contract FirmwareUpdates (owner-only switch between Temperature and Humidity modes; emits FirmwareUpdate event) GitHub | EVM chain (tested on Ganache & Infura PoA network)

