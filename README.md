# TCP Chat Application

This repository contains a simple TCP-based chat application implemented in C. The application consists of two components:

1. **Chat Server**: Handles multiple client connections and facilitates message broadcasting.
2. **Chat Client**: Allows users to connect to the server and exchange messages with other clients.

## Features

- Multiple client support using `select()` for I/O multiplexing.
- User identification with usernames.
- Message broadcasting to all connected clients.
- Graceful handling of client disconnections.

## Files

- `tcpchatserver.c`: The server-side implementation.
- `tcpchatclient.c`: The client-side implementation.

## Compilation

To compile the server and client programs, use the `gcc` compiler. Run the following commands in your terminal:

```bash
# Compile the server
gcc tcpchatserver.c -o tcpchatserver

# Compile the client
gcc tcpchatclient.c -o tcpchatclient
```

## Usage

### Running the Server

1. Start the server by running the following command:

   ```bash
   ./tcpchatserver
   ```

   The server will start listening on the default port `8080`.

### Running the Client

1. Start the client by running the following command:

   ```bash
   ./tcpchatclient
   ```

2. Enter the port number to connect to (default is `8080`).
3. Enter your username.
4. Start chatting!

### Exiting the Client

To exit the client, type `exit` and press Enter.

## Notes

- Ensure that the server is running before starting the client.
- The server and client are configured to communicate over `127.0.0.1` (localhost). Modify the IP address in the client code if you want to connect to a remote server.

## Dependencies

- `gcc` compiler
- Standard C libraries