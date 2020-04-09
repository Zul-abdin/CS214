#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

int main() {
    //Server socket: Waits and listens for connections from client

    /*
     * Networking:
     * - Always use threads when networking, for the case when multiple clients want to connect
     *   - accept is a blocking call, so switching between clients is not useful, the only way to reliably deal with multiple connections, is threads
     *   - One main thread that listens for clients
     *   - Server-side creates a new thread for every client connection to handle them
     *     - clientHandlerFunction(int fd) --> void* clientHandlerFunction(void* fd)
     *
     * - Server
     *   - Networking Code running on some host
     *   - Usually runs/exists before client runs
     *   - Waits for clients to connect to it
     *   - reads some command from client on connect, then sends something back
     * - Client
     *   - Networking Code running on some host
     *   - Usually runs/exists after client runs
     *   - Connects to a server
     *   - writes some command/request to server, then reads/interprets the response
     * - Server Socket
     *   - Can only accept connection requests
     *   - Setting up Server Socket:
     *     - socket()
     *     - Build annoying struct to represent server socket to connect to
     * - Client Socket
     *   - Can read/write bytes/data
     *   - Setting up Client Socket:
     *     - socket()
     *     - Build annoying struct to represent server socket to connect to
     *     - Need to know:
     *       - IP Address: Identify host machine
     *       - Port Number: Identify process on host
     * - Setting up server-side
     *   - Build a socket - socket()
     *     - Returns a client socket connected to the client-side, so that the server-side can use it to send/recieve data from the client-side
     *   - bind the socket to a port number - bind()
     *   - Make it a listening-type (server) socket - listen()
     *   - Start listening for connection requests - accept()
     * - Setting up client-side
     *   - Build a socket - socket()
     *   - Connect to a server - connect()
     *
     * - Coding Sockets
     *   - Client-Side:
     *     - socket(<Address Family>, <Transport/Socket Protocol>, <Options>)
     *     - socket(AF_INET, SOCK_STREAM, 0)
     *       - TCP Connection
     *     - getaddrinfo(...)
     *       - Gives all possible port binds for a specific socket, on ...
     *       - Gives you much more information than you need, and has a much more complex set of arguments
     *       - NOTE: getaddrinfo(...) is the preferred version to use
     *     - gethostbyname(...)
     *       - More Direct, gives you less information, but gives you what you need
     *
     * - Host
     *   - Machine on the network
     * - Switch
     *   - Can relay information between hosts that it is directly connected to
     * - Router
     *   - Can relay information between hosts that are not directly connected to it
     *   - Builds (routes) a path between other routers and the host to connect itself to a different host
     *   - Relays information through this indirect connection
     *
     * - IP Address - identifies a given host (machine/device) on the network
     *   - Format: Dotted Octets, From 000.000.000 to 255.255.255.255
     * - Port Number - identifies which Process on a given host to relay data/a message to
     *
     * - ISO OSI: 7-layer stack to generalize networking
     *   - Application layer: most abstract layer IO
     *   - Presentation Layer
     *   - Session Layer: sync info between multiple connections
     *   - Transport Layer
     *   - Networking Layer (ex. router): Determine a route
     *   - Datalink Layer (ex. switches): Point-to-point sending to
     *   - Physical Layer:
     *
     *
     * Special File Descriptors Values
     * - STDIN: 0
     * - STDOUT: 1
     * - STDERR: 2
     */


    /*
     * Internet: TCP/IP
     * - Internetworking Protocol (IP)
     *   - routing to/finding a given machine
     *   - represented by four dotted octets:
     *     - 255.255.255.255 - Identifies machines
     */

    int sfd = socket(AF_INET, SOCK_STREAM, 0);

    //Get IP address/host name translates into a useful format
    struct hostent* result = gethostbyname("<Some address or hostname>");

    //make the super-special struct that C requires for connect()
    struct sockaddr_in serverAddress;

    //zero it out
    bzero(&serverAddress, sizeof(serverAddress));

    //set the addr family (internet)
    serverAddress.sin_family = AF_INET;

    //set port number to connect to... also, convert numeric type to a general 'Network short' type
    //Host-to-network-short
    serverAddress.sin_port = htons(7621);

    //copy the RAW BYTES from the hostent struct in to the sockarrd_in one
    bcopy((char*) result->h_addr, (char*)&serverAddress.sin_addr.s_addr, result->h_length);

    connect(sfd, &serverAddress, sizeof(serverAddress));

    return 0;
}
