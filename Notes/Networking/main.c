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
     * - ISO OSI: 7-layer stack to generalize networking: Higher Layers Use Lower Layers as a service (Every Layer does its operation and its duel/opposite (vice-versa)), created using OOD (Object-Oriented Design)
     *   <--- HIGHEST LAYER
     *   - Application layer: Most abstract layer, Interacts with user and v.v (ex. UI)
     *   - Presentation Layer: Modify/Process data to make it useful and v.v (ex. en-/de-coding, en-/de-cryption, en-/de-compression)
     *   - Session Layer: Manages messages over a number of connections and v.v (ex. sync, timing, multiplexing, sequencing)
     *     - Gets information from Transport layer, using it like a service to get info from Data Link Layer, but does not know how transport layer works
     *   - Transport Layer: first end-to-end Layer, (ex. sockets are transport-layer structures/abstractions)
     *     - Manages messaging and v.v (getting all of message the the other side quickly, reliably, and efficiently)
     *     - Pieces together information from the Networking Layer and gives it to the session layer requesting information and v.v
     *   - Networking Layer: Determine a route and v.v (ex. router)
     *     - Manages delivery to hosts not directly connected to and v.v (builds/maintains routes, relays messages)
     *   - Data Link Layer: Point-to-point sending to (ex. switches)
     *     - Manages delivery to hosts directly connected to and v.v (error check/encoding, identity)
     *   - Physical Layer: Manages translation of bits into something physical that propagates, and v.v. (rate, resiliency, bandwidth)
     *   <--- LOWEST LAYER
     *
     * - OOD (Object Oriented Design)
     *   - Encapsulation
     *   - Abstraction
     *   - Modularity
     *
     * - Routing
     *   - Routes implementing using partial 'store and forward'
     *     - routers forward a message to some machine they are directly connected to that is 'closer' to the target than they are
     *   - IP Address Design
     *     - XXX.X.X.XXX
     *       - The first octet represents a large area
     *       - Every next octet represents an increasingly smaller amount of area
     *     - Class Design: Became outdated, didn't separate addresses nicely
     *       - All octets that are ___ can be set by the owner
     *       - Class A: 128.___.___.___
     *       - Class B: 128.5.___.___
     *       - Class C: 128.5.6.___
     *       - Class D: 128.5.6.129
     *       - What if you want 4 addresses? You would need to get 4 completely different class D's...
     *     - CIDR: Classless InterDomain Routing
     *       - 128.5.6.129/x where the first x bits are set
     *       - 128.5.6.129/32 means you only have one address (you can vary the last 2 bits in only 1 way)
     *       - 128.5.6.129/30 means you have 4 addresses (you can vary the last 2 bits in 4 ways)
     *       - Now to route a message, you just have to route to the router with the most widest matching bits
     *       - When routing to far away places, usually the router will build a path to increasingly inexact addresses, from /24 to /10 to /8 ... for example.
     *       - This inexact router is able to route more of the network.
     *       - Then eventually you will be at the far supervisor of the router you want, and starting with the far supervisor will go to increasingly exact addresses
     *     - Tiers of Routing
     *       - L1
     *       - L2
     *       - L3
     *       - Regional/Organizational (Big ISP)
     *       - Local (ISP)
     *       - Personal <-- You
     *     - DNS (Domain Name Service)
     *       - Service that runs on top of the internet
     *       - Translates human-readable names into network-usable IP Addresses
     *       - Types of Domain servers
     *         - root
     *         - Authoritative name server (ex. rutgers.edu)
     *         - non-Authoritative name server - not local
     *       - Locally cached lookups
     *       - On DNS lookup if not in local cache:
     *         - recursive: if the name server doesn't know, it checks the next level up
     *         - non-recursive: if the name server doesn't know, it tell you who to contact (IP addr of next level)
     *       - "www." world-wide web... canonical sub-domain to represent web servers only
     *         - (back when you would encode the service you wanted from a domain in the domain name itself)
     *     - DNS and IP allocation/maintenance
     *       - IANA (Internet Authority on Naming and Addressing)
     *       - ICANN (Internet Consortium on Addressing Naming and Numbering)
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
