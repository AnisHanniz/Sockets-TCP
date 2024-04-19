#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define TAILLE_BUFFER 256
#define error(...) { fprintf(stderr, __VA_ARGS__); fflush(stderr); exit(1); }
#define DIR "sockets/"

int create_client_socket(const char *path_socket) {
    int fd_client = socket(AF_UNIX, SOCK_STREAM, 0);
    if (fd_client < 0) error("Impossible de créer la socket\n");

    struct sockaddr_un serveur;
    serveur.sun_family = AF_UNIX;
    strncpy(serveur.sun_path, DIR, sizeof(serveur.sun_path));
    strncpy(serveur.sun_path + strlen(DIR), path_socket, strlen(path_socket) + 1);

    int err = connect(fd_client, (struct sockaddr*)&serveur, sizeof(serveur));
    if (err < 0) error("Impossible de se connecter au serveur\n");

    printf("Connecté au serveur sur %s\n", path_socket);

    return fd_client;
}

int main(int argc, char *argv[]) {
    if (argc < 2) error("Utilisation : %s [chemin-socket]\n", argv[0]);

    char *chemin_socket = argv[1];

    int fd_client = create_client_socket(chemin_socket);

    while (1) {
        printf("Entrez un message (ou 'exit' pour quitter) : ");
        char buf[TAILLE_BUFFER];
        fgets(buf, TAILLE_BUFFER, stdin);

        // supprime le caractère de nouvelle ligne final
        size_t len = strlen(buf);
        if (len > 0 && buf[len - 1] == '\n') {
            buf[len - 1] = '\0';
        }

        if (strcmp(buf, "exit") == 0) {
            break;
        }

        int octets_sent = send(fd_client, buf, strlen(buf), 0);
        if (octets_sent < 0) error("Écriture vers le serveur échouée\n");

        int octets_recv = recv(fd_client, buf, TAILLE_BUFFER, 0);
        if (octets_recv < 0) error("Lecture du serveur échouée\n");

        buf[octets_recv] = '\0'; //fin des données reçues
        printf("Réponse du serveur : %s\n", buf);
    }

    close(fd_client);

    return 0;
}
