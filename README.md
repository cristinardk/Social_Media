TEMA REALIZATA DE

Iordache Cristina (UUID: Ozohghioquai)

Dobre Andrei-Teodor (UUID: ohtooBohghae)

Acest proiect implementeaza o retea de socializare simplificata, incluzand
gestionarea utilizatorilor, prietenilor, postarilor si repostarilor.
Proiectul este compus din mai multe module care acopera diverse
functionalitati, cum ar fi adaugarea si eliminarea prieteniilor, crearea
postarilor, generarea feed-ului, si afisarea profilului utilizatorilor.

## TASK 1
Acesta parte a proiectului descrie functiile implementate pentru administrarea
prietenilor intr-o retea de utilizatori. Functiile includ adaugarea si
eliminarea de prietenii, sugestii de prieteni, calcularea distantei minime
intre doi utilizatori, gasirea prietenilor comuni, numarul de prieteni si
gasirea celui mai popular prieten.

In implementarea urmatoarelor functii am folosit si functii legate
de coada si graf cum ar fi creare, adaugare, eliminare, etc.,
functiile fiind scrise intr-un mod general.

*Functii de administrare a prieteniilor*

+ void add_friendship(list_graph_t *graph, char *user1, char *user2) :
Adauga o prietenie intre doi utilizatori.

+ void remove_friendship(list_graph_t *graph, char *user1, char *user2) :
Elimina o prietenie intre doi utilizatori.

+ void print_min_path(list_graph_t *graph, char *user1, char *user2) :
Afiseaza distanta minima intre 2 utilizatori

+ void suggestions(list_graph_t *graph, char *name) :
Afiseaza sugestii de prieteni pentru un utilizator

+ void print_common_friends(list_graph_t *graph, char *name1, char *name2) :
Afiseaza prietenii comuni intre doi utilizatori

+ void print_friends_count(list_graph_t *graph, char *name):
Afiseaza numarul de prieteni ai unui utilizator

+ void print_most_popular_friend(list_graph_t *graph, char *name) :
Afiseaza cel mai popular prieten al unui utilizator

## TASK 2
Acest task descrie functiile implementate pentru administrarea postarilor
intr-o retea de utilizatori. Functiile includ crearea de postari, repostari,
administrarea postarilor si a repostarilor, gestionarea aprecierilor
(like-urilor) si afisarea postarilor si repostarilor.

*Crearea si Administrarea Arborilor*

+ tree_node_t *create_tree_node(int pid, void *data) :
Creeaza un nod de arbore cu ID-ul si datele specificate.

+ tree_t *create_tree(int data_size) :
Creeaza un arbore cu dimensiunea specificata a datelor.

+ void add_child(tree_node_t *parent, tree_node_t *child) :
Adauga un copil la un nod de arbore.

+ tree_node_t *find_node(tree_t *tree, int id) :
Gaseste un nod in arbore dupa ID.

+ tree_node_t *find_node_recursive(tree_node_t *node, int id) :
Gaseste un nod in arbore in mod recursiv dupa ID.

+ void free_tree_node(tree_node_t *node) :
Elibereaza memoria alocata unui nod de arbore si copiilor sai.

+ void free_tree(tree_t *tree): Elibereaza memoria alocata unui arbore.

*Crearea si administrarea postarilor*

+ void ll_add_post_node(linked_list_t *list, unsigned int n, void *new_data):
Adauga un nod de postare intr-o lista de postari.

+ post *create_post(char *title, int uid, int *contor_id) :
Creeaza o postare cu titlul si ID-ul utilizatorului specificate.

+ void create_post_command(const char *user_name, char *title,
int uid, linked_list_t *lists_of_posts, int *contor_id) :
Creeaza o postare pe baza unei comenzi.

+ post *create_repost(int uid, int *contor_id, char *original_title) :
Creeaza o repostare cu titlul original si ID-ul utilizatorului specificat.

+  void repost_command(linked_list_t *lists_of_posts, const char *user_name,
int post_id, int uid, int repost_id, int *contor_id) :
Creeaza o repostare pe baza unei comenzi.

*Afisarea si Gestionarea Postarilor si Repostarilor*

+ void print_reposts(tree_node_t *node, int level, int contor_id):
Afiseaza repostarile unui nod de arbore.

+ void get_reposts_command(linked_list_t *lists_of_posts, int post_id,
int repost_parent_id, int *contor_id) :
Obtine si afiseaza repostarile pe baza unei comenzi.

*Aprecierea Postarilor*

+ void like_post(post *p, int user_id) :
Adauga sau elimina un like de la un utilizator pentru o postare.

+ void like_command(linked_list_t *lists_of_posts, int post_id,
int repost_id, int uid): Adauga sau elimina un like pe baza unei comenzi.

+ void get_likes_command(linked_list_t *lists_of_posts,
int post_id, int repost_id) :
Obtine si afiseaza numarul de like-uri pentru o postare sau repostare.

*Gasirea Postarilor Comune si Raportarea*

+ post *find_most_liked_repost(tree_node_t *root):
Gaseste repostarea cu cele mai multe like-uri intr-un arbore de repostari.

+ tree_node_t *find_common_ancestor(tree_node_t *root, tree_node_t *node1,
tree_node_t *node2) :
Gaseste stramosul comun pentru doua repostari intr-un arbore.

+ void common_repost_command(linked_list_t *lists_of_posts, int post_id,
int repost_id1, int repost_id2) :
Obtine repostarea comuna pentru doua repostari pe baza unei comenzi.

+ void ratio_command(linked_list_t *lists_of_posts, int post_id) :
Raporteaza daca o repostare are mai multe like-uri decat postarea originala.

*Stergerea Postarilor si Respostarilor*

+ void remove_child(tree_node_t *parent, tree_node_t *child) :
Elimina un copil dintr-un nod de arbore.

+ void delete_post_command(linked_list_t *lists_of_posts, int post_id,
int repost_id): Sterge o postare sau repostare pe baza unei comenzi.

## TASK 3
Acesta secventa din proiect descrie functiile implementate pentru
administrarea feed-ului intr-o retea de utilizatori. Functiile includ
generarea feed-ului unui utilizator, afisarea profilului unui utilizator,
afisarea repostarilor prietenilor si identificarea grupului comun de prieteni.

*Generarea Feed-ului*

+ void feed_command(list_graph_t *graph, char *user_name, int feed_size,
linked_list_t *lists_of_posts) :
Genereaza feed-ul unui utilizator specificat cu o dimensiune maxima data.

*Afisarea Profilului*

+ void view_profile_command(char *user_name, linked_list_t *lists_of_posts) :
Afiseaza profilul unui utilizator, incluzand postarile si repostarile acestuia.

*Afisarea Repostarilor Prietenilor*

+ void friends_repost_command(list_graph_t *graph, char *user_name,
int post_id, linked_list_t *lists_of_posts) :
Afiseaza repostarile unui post specificat facute de prietenii unui utilizator.

*Grupul Comun de Prieteni*
+ void common_group(list_graph_t *graph, char *user_name) :
Identifica si afiseaza grupul comun de prieteni al unui utilizator.

*Functii Suport*

+ void find_and_print_reposts_by_user(tree_node_t *root, int user_id) :
Gaseste si afiseaza repostarile facute de un utilizator
intr-un arbore de evenimente.

+ void find_and_print_friends_reposts(tree_node_t *root, int user_id, int friend_id) :
Gaseste si afiseaza repostarile facute de prietenii unui utilizator
intr-un arbore de evenimente.

+ void bronkerbosch1(int *R, int r_size, int *P, int p_size, int *X,
int x_size, list_graph_t *graph, int ***cliques, int *clique_count,
int *max_cliques_size) :
Algoritmul Bron-Kerbosch pentru gasirea clicurilor maxime intr-un graf.

+ int **find_maximal_cliques(list_graph_t *graph, int *clique_count) :
Gaseste clicurile maxime intr-un graf folosind algoritmul Bron-Kerbosch.

+ void free_cliques(int **cliques, int clique_count) :
Elibereaza memoria alocata clicurilor maxime.

SURSE :
1. https://en.wikipedia.org/wiki/Bron%E2%80%93Kerbosch_algorithm
Am utilizat acest algoritm pt gasirea celei mai mari clici
2. Laborator de liste
3. Laborator de queue
4. Putin si lab - urile de grafuri dar doar asa orientativ pt ca
ni s-a parut mai usor sa ne facem cum vrem structura si functiile

!!!DISCLAIMER!!! la ultimul test cred ca problema este la functia de delete
pt ca am pus repostarile unui repost in repost->children in loc de
original_post->children si la delete nu se mai vad copii ascendenti de
dedesubtul celui sters. Ar fi ajutat la debugging daca testul nu avea
1500 de linii :))
