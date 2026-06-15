# IPK Reliable Data Transfer (RDT) Protocol

## Overview

This project implements a reliable data transfer protocol over UDP. Since UDP does not guarantee delivery, ordering, or duplication protection, this implementation adds these features at the application layer.

The protocol ensures:

* reliable delivery
* ordered delivery
* no duplicated data in output
* robustness against packet loss, duplication, and reordering

---

## Protocol Design

### Packet / Header Format

Each packet consists of a fixed-size header followed by payload:

```
+----------------+----------------+----------------+----------------+
|   seq (4B)     |   len (2B)     |  flags (2B)    | checksum (4B)  |
+----------------+----------------+----------------+----------------+
|                         data (up to 1024B)                         |
+----------------------------------------------------------------+
```

* `seq` – sequence number of the packet
* `len` – payload size in bytes
* `flags` – packet type (DATA, ACK, FIN)
* `checksum` – integrity protection over header + payload

All multi-byte fields are transmitted in network byte order.

---

### Integrity Protection

A 32-bit checksum is computed over the serialized packet (header + payload), with the checksum field zeroed during computation.

On reception:

* checksum is extracted
* checksum field is zeroed
* checksum is recomputed
* packet is accepted only if values match

Corrupted packets are silently discarded.

---

## Session Management

### Session Establishment

The protocol does not implement an explicit handshake (e.g., SYN). A session is implicitly established when the first valid packet is received.

Invalid or malformed packets before session establishment are ignored.

---

### Session Termination

Connection termination is handled using a FIN mechanism:

1. Sender transmits a FIN packet
2. Receiver replies with a FIN acknowledgment
3. Both sides terminate after successful exchange

Retransmissions are used to ensure FIN delivery.

---

## Data Transfer

### Sequencing and Acknowledgements

* Each packet is assigned a monotonically increasing sequence number
* Receiver acknowledges packets using ACK packets carrying the sequence number
* Sender tracks acknowledgements per packet

---

### Sliding Window

A fixed-size sliding window is used:

* Sender maintains:

  * `w_base` – first unacknowledged packet
  * `w_next` – next packet to send
* Receiver maintains a buffer for out-of-order packets

Window constraints:

```
w_next - w_base < WINDOW_SIZE
```

---

### Out-of-Order Handling

Receiver:

* stores packets within window range
* marks received packets
* delivers data only when all previous packets are received

This ensures strictly ordered output.

---

### Duplicate Handling

* Receiver ignores already received packets
* ACKs are still sent for duplicates
* Duplicate data is never written to output

---

## Retransmission Strategy

### Timeout Handling

* Each packet has its own retransmission timer
* If ACK is not received within `RESEND_TIMEOUT`, packet is retransmitted

### Global Timeout (-w)

* Tracks **protocol progress**, not just activity
* Progress includes:

  * new data received
  * new ACK received
  * successful handshake or termination step

If no progress occurs within `TIMEOUT` seconds:

* application terminates with non-zero exit code

---

## Connection Identification

The implementation assumes a single session per process execution.

Packets are associated with a session implicitly via:

* source IP address
* source port

Packets from unrelated sessions are ignored.

---

## Segment Size and Window Behavior

* Payload size: 1024 bytes
* Header size: 12 bytes
* Window size: fixed (10)

Trade-offs:

* larger payload → better throughput
* smaller payload → better loss recovery

---

## Measured Behavior

Testing was performed on localhost with simulated network conditions.

### Observed properties:

* No duplicated output
* Reliable termination via FIN

---

### Sender State (simplified)

```
[START]
   ↓
[SEND DATA] → [WAIT FOR ACK]
   ↑             ↓
   └── retransmit on timeout
   ↓
[ALL ACKED]
   ↓
[SEND FIN]
   ↓
[WAIT FIN-ACK]
   ↓
[END]
```

---

### Receiver State (simplified)

```
[WAIT DATA]
   ↓
[RECEIVE PACKET]
   ↓
[VALIDATE CHECKSUM]
   ↓
[STORE / DISCARD]
   ↓
[SEND ACK]
   ↓
[IN-ORDER DELIVERY]
   ↓
[FIN RECEIVED]
   ↓
[SEND FIN-ACK]
   ↓
[END]
```

---

## Known Limitations

* No explicit handshake (SYN phase)
* Single session per process execution
* No congestion control (fixed window size)
* No adaptive retransmission timing
* Assumes reasonably bounded packet reordering
* Problems occur when combining transmitting large files with packet duplication
---

## Usage

### Server

```
./ipk-rdt -s -p PORT [-a ADDRESS] [-o OUTPUT] [-w TIMEOUT]
```

### Client

```
./ipk-rdt -c -a HOST -p PORT [-i INPUT] [-w TIMEOUT]
```

---
## AI Usage
* This readme was largely generated using ai
* Tests were generated using ai
* Function for computing checksum was generated using ai
