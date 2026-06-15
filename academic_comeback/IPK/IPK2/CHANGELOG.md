# Changelog

### Added
- Sliding window sender implementation
- Basic ACK handling and retransmission logic
- Packet serialization/deserialization
- FIN handshake for connection termination

### Known limitations
- Problems occur when combining transmitting large files with packet duplication
- Packet loss scenarios not properly tested under stress
- Retransmission and timeout behavior may be unreliable under high loss or latency