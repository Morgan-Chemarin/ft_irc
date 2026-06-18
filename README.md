*This project has been created as part of the 42 curriculum by mchemari, pibreiss*

---

# ft_irc — Internet Relay Chat Server

## Description

ft_irc is our implementation of an IRC server, written in **C++98** and compliant with **RFC 2812**.

The goal: to understand how a network server actually works—socket management, connection multiplexing, and protocol parsing. We use only a single thread, and `poll()` does all the work to listen simultaneously to the server and all connected clients, without ever blocking.

---

### Supported Commands

---

**Authentication & Connection**
- `PASS` - verifies the server password
- `NICK` - sets or changes the nickname (case-insensitive, max 9 characters)
- `USER` - completes user registration
- `CAP` - negotiates network capabilities (immediate compatibility with HexChat)

**Channel Management**
- `JOIN` - join one or more channels (supports `/JOIN 0` to leave all channels at once)
- `PART` - leave a channel with an optional message
- `TOPIC` - display or change the channel topic
- `INVITE` - invite someone to a private channel
- `KICK` - kick a member (operators only)

**Communication**
- `PRIVMSG` - private messages between users or broadcast messages on a channel
- `PING` - keep the connection alive

**Channel Modes (`MODE`)**
- `+i` / `-i` - invitation-only access
- `+t` / `-t` - topic editing restricted to operators
- `+k` / `-k` - channel access password
- `+o` / `-o` - grant or revoke operator status (`@`)
- `+l` / `-l` - maximum user limit

---

## Instructions

### Prérequis

- A `c++98` compatible C++ compiler
- An IRC client, we recommend **HexChat** for convenient testing

### Compile

```bash
make
```

Generates the `ircserv` executable with the flags `-Wall -Wextra -Werror -std=c++98`.

### Lancer le serveur

```bash
./ircserv <port> <password>
```

Exemple :
```bash
./ircserv 6667 my_secret_password
```

The port must be between **1024 and 65535**.

### Connect with NetCat (quick test)

```bash
nc -C localhost <port>
```

Then authenticate manually:
```
PASS my_secret_password
NICK myusername
USER myusername 0 * :My Name
```

### Connecting with HexChat

1. Open HexChat, Network, add a network pointing to `localhost/6667`
2. Enter the password set at startup
3. Connect

---

## Project Architecture

```
.
├── includes/
│   ├── Server.hpp         # Network core (socket, bind, listen, poll)
│   ├── Client.hpp         # Representation and state of a connected user
│   ├── Channel.hpp        # Chat room logic and modes
│   ├── ACommand.hpp       # Abstract interface for IRC commands
│   ├── Parser.hpp         # Parsing and extracting IRC messages
│   └── Command[X].hpp     # Headers for each specific command
├── srcs/
│   ├── main.cpp           # Entry point, signal handling
│   ├── Server.cpp
│   ├── Client.cpp
│   ├── Channel.cpp
│   └── commands/          # Command implementation (Join, Nick, Part…)
├── Makefile
└── README.md
```

**Some notable technical choices:**
- **Non-blocking `poll()`** - a single thread handles server listening and all client activity in `O_NONBLOCK` mode
- **Command Pattern** - each IRC command is encapsulated in its own class, stored in a `std::map`; easy to integrate, easy to test
- **Reply Builder** - a dedicated builder for constructing standardized IRC replies without having to rewrite the formatting each time

---

## Resources

### Documentation & References

- [RFC 2812 — Internet Relay Chat: Client Protocol](https://www.rfc-editor.org/info/rfc2812) - the official specification, essential
- [RFC 2812 §2.3.1 — Message format](https://www.rfc-editor.org/info/rfc2812/#section-2.3.1) - the exact format of IRC messages
- [Tutoriel IRC — Le protocole (developpez.com)](https://mathieu-lemoine.developpez.com/tutoriels/irc/protocole/?page=connexion) -  a clear explanation of the protocol in French
- [RFC IRC en français (iprelax.fr)](http://www.iprelax.fr/irc/irc_rfcfr6.php) - a partial translation of the RFC, useful for getting up to speed quickly

### Using AI

We used Claude mainly to **coordinate our efforts as a team**: defining who does what, planning the project’s milestones, and avoiding stepping on each other’s toes. It served as a shared dashboard to structure our approach.

We also used it occasionally for **debugging** (understanding unexpected network behavior, checking edge cases in the RFC) and for writing this README.