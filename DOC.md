```c#
int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
```
cree la socket AF_INET = IPv4 / SOCK_STREAN = TCP 
return fd de la socket

```c#
setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
```
quand on coupe le serveur irc  et quon relance le port est bloque un peu de temps. Ca e;peche d afficher un msg chiant

```c#
struct sockaddr_in address;
address.sin_family = AF_INET;
address.sin_addr.s_addr = INADDR_ANY;
address.sin_port = htons(port);
```
cest la structure avec les coordonnees du serveur: 
- AF_INET = IPv4 
- INADDR_ANY = le serveur ecoute sur toutes les cartes reseaux
- htons() = converti le port en language reseaux

```c#
bind(serverSocket, (struct sockaddr *)&address, sizeof(address));
```
on va lie la socket avec la config de l adress donc le port est reserve pour le programme

```c#
listen(serverSocket, 1);
```
- 1 = la fille d attente ?


```c#
int clientSocket = accept(serverSocket, ...);
```
le socket sert a attendre que quelquun arrive 
le programme va mettre pause sur cette ligne tant que ya pas de client connecte
dcp cest ca le mode bloquant
faut pas faire ca sinon ca bloque les autres connectes 

# Instruction
- make
- ./ircserv "port" "password"
- nc localhost "port"