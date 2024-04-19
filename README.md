# Serveur de sockets TCP

Ce projet consiste en un serveur de sockets basique implémenté en langage C, capable de recevoir des messages des clients et de leur renvoyer ces messages.

## Fonctionnalités

- Accepte les connexions de plusieurs clients simultanément.
- Reçoit et renvoie des messages aux clients.
- Gestionnaire de signal pour arrêter proprement le serveur.
- Fichier `server_logs.txt` pour tracer les messages transférés.
- Dossier sockets pour stocker les sockets creés.

## Logs

Les messages échangés avec les clients sont enregistrés dans le fichier logs/server_logs.txt.

## Compilation

Pour compiler le projet, exécutez les commandes suivantes dans votre terminal :

 `make` pour compiler
 
 `make clean` -> pour effectuer un nettoyage

Cela générera les exécutables `serveur` et `client`.

## Utilisation

1. Lancez le serveur en spécifiant le chemin du socket :

./serveur [chemin-socket]

2. Lancez un ou plusieurs clients dans d'autres terminaux :

./client [chemin-socket]

3. Suivez les instructions à l'écran pour envoyer des messages depuis les clients vers le serveur et vice versa.

4. Pour arrêter le serveur, utilisez `Ctrl + C`.

## Structure des fichiers

- `serveur.c`: Code source du serveur.
- `client.c`: Code source du client.
- `README.md`: Ce fichier.
- `makefile`
