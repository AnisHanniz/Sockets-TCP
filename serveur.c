#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

#define TAILLE_BUFFER 256
#define error(...) { fprintf(stderr, __VA_ARGS__); fflush(stderr); exit(1); }
#define LOG_FILENAME "logs/server_logs.txt"
#define DIR "sockets/"
void help() {
    printf("\t\t-----------Bienvenue dans notre serveur de sockets !-----------\n");
    printf("\tIl vous suffit maintenant de lancer un ou plusieurs clients\n");
    printf("\tdans d'autres terminaux avec la commande './client', \n");
    printf("\tpuis vous n'aurez plus qu'à inscrire et entrer vos commandes ! \n");
    printf("\tVous pouvez interrompre à tout moment avec Ctrl + C. \n");
    printf("\t\t----------------------------------------------------------------\n");
}


void log_message(const char *message) {
    FILE *log_file = fopen(LOG_FILENAME, "a");
    if (log_file == NULL) {
        perror("Erreur lors de l'ouverture du fichier de logs");
        return;
    }

    fprintf(log_file, "%s\n", message);
    fclose(log_file);
}


void gestionnaire(int signum) {
    if (signum < 0) {
        fprintf(stderr, "Mauvais signal\n");
        exit(EXIT_FAILURE);
    }
    fprintf(stdout, "\nArrêt du processus...\n");
    exit(0);
}

int create_socket_server(const char *path_socket) {
    int fd_serveur = socket(AF_UNIX, SOCK_STREAM, 0);
    if (fd_serveur < 0) error("Impossible de créer la socket\n");

    struct sockaddr_un serveur;
    serveur.sun_family = AF_UNIX;
    strncpy(serveur.sun_path, DIR, sizeof(serveur.sun_path));
    strncpy(serveur.sun_path + strlen(DIR), path_socket, strlen(path_socket) + 1);
    unlink(serveur.sun_path);

    int err = bind(fd_serveur, (struct sockaddr *)&serveur, sizeof(serveur));
    if (err < 0) error("Impossible de lier la socket\n");

    err = listen(fd_serveur, 128);
    if (err < 0) error("Impossible d'écouter sur la socket\n");

    printf("Le serveur écoute sur %s\n",path_socket);

    return fd_serveur;
}

int main(int argc, char *argv[]) {
    help();

    struct sigaction act; 
    act.sa_handler = gestionnaire;
    act.sa_flags = 0;
    if (sigfillset(&act.sa_mask) == -1) {
        perror("sigfillset");
        exit(EXIT_FAILURE);
    }
    if (sigaction(SIGINT, &act, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    if (argc < 2) error("Utilisation : %s [chemin-socket]\n", argv[0]);

    char *chemin_socket = argv[1];

    int fd_serveur = create_socket_server(chemin_socket);

    while (1) {
        struct sockaddr_un client;
        socklen_t taille_client = sizeof(client);

        int fd_client = accept(fd_serveur, (struct sockaddr *)&client, &taille_client);
        if (fd_client < 0) {
            perror("Impossible d'établir une nouvelle connexion\n");
            log_message("Impossible d'établir une nouvelle connexion\n");
            continue;
        }

        pid_t pid = fork();

        switch (pid) {
            case -1:
                perror("Erreur lors de la création d'un processus fils\n");
                close(fd_client);
                log_message("Lecture du client échouée");
                break;

            case 0:
                close(fd_serveur);
                while (1) {
                    char buf[TAILLE_BUFFER];
                    int octets_lus = recv(fd_client, buf, TAILLE_BUFFER, 0);

                    if (octets_lus <= 0) {
                        if (octets_lus < 0) error("Lecture du client échouée\n");
                        log_message("Écriture vers le client échouée");
                        break;
                    }

                    printf("Je viens de recevoir : %.*s\n", octets_lus, buf);

                    log_message(buf);

                    int octets_sent = send(fd_client, buf, octets_lus, 0);
                    if (octets_sent < 0) {error("Écriture vers le client échouée\n");
                        log_message("Écriture vers le client échouée");}
                }
                close(fd_client);
                exit(0);

            default:
                close(fd_client);
                break;
        }

        int status;
        waitpid(-1, &status, WNOHANG);
    }

    return 0;
}
