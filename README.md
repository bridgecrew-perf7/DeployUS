# DeployUS
Par: **Shawn Vosburg (voss2502)**  
Équipe: EQ02  
Email: shawn.vosburg@[usherbrooke.ca, mail.mcgill.ca]  
Date: 12 avril 2021  
<br>
Dans le cadre du cours IGL711 de l'Université de Sherbrooke.  
Partie réalisé jusqu'à présent: Toutes les phases + bonus (client graphique)

### Avancement de mi-projet 
J'ai décidé d'y aller de l'avant avec **Python** pour DeployUS car je suis déjà familier avec le dévelopment web avec Flask. Flask contient Jinja2, qui me permet de faire des templates HTML et recycler facilement du code html. Aussi, il permet d'insérer du code python en html, ce qui m'est très utile pour déveloper le client.

Pour WorkUS et WatchUS, je vais y aller de l'avant avec **Go** car c'est seulement 2 endpoints que je dois coder. C'est si simple que faire un projet C# ou C++ serait trop complexe. Avec Go, un seul fichier est nécessaire. 

### Introduction
DeployUS est une agglomération de microservice qui performe le déploiement
d'applications *docker-compose* sur des machines distantes via REST API. Un client
graphique a été développé pour en facilité l'utilisation.   

DeployUS est conçu pour être installer sur des serveurs distant et être accèdé 
via un navigateur web. Cet outil est idéal pour le déploiement d'application web.

Le micro-service DeployUS est écrit entièrement en Python.  
Le micro-service WatchUS est écrit GO avec un script de lancement des tests en Python.  


### Dépendances
Pour executer DeployUS, les machines hôtes doivent avoir ces programmes suivants.
1. docker
2. docker-compose

### Machines distantes
J'utilise les machines virtuelles de Google (https://cloud.google.com/).  
Les machines ont Ubuntu 20.04 LTS avec 4GB de RAM.  
Tous les ports sont ouverts pour permettre une présentation plus verbose aux correcteurs.


# Micro-service DeployUS

### Bibliothèques externes
* Le micro-service DeployUS est ecrit avec le langage de programmation Python3.
* Pour le serveur web, la bilbiotèque Flask (https://pypi.org/project/Flask/) est utilisée. Flask contient Jinja2, qui permet de facilement recycler du code HTML sur plusieurs pages. Licence BSD (BSD-3-Clause).
* Pour connecter avec la base de données MySQL, j'utilise la librairie mysql-connector (https://pypi.org/project/mysql-connector/). Licence MIT.
* Pour executer les tests, j'utilise pytest (https://pypi.org/project/pytest/). Licence MIT.
* Pour faire un mock des WorkUS, j'utilise pytest_httpserver (https://pypi.org/project/pytest-httpserver/). Licence MIT.
* Pour faire de requêtes REST dans mes tests, j'utilise la librairie requests (https://pypi.org/project/requests/). Licence Apache Software License (Apache 2.0). 
* L'analyse de code statique est fait à l'aide des modules pylint (https://github.com/cytopia/docker-pylint) et pycodestyle (https://github.com/cytopia/docker-pycodestyle). Licence MIT.

### Fonctionnement
DeployUS utilise des requêtes REST pour recevoir des scripts valides de docker-compose.yml sur le port 5000.  
Ces scripts doivent:
* avoir un nom qui fini par .yml
* être valid selon « docker-compose config »

L'utilisateur peut ensuite déclarer des machines distantes comme « worker » et 
déployer leur script sur ces machines. Présentement, seulment la machine locale
(i.e. localhost) est disponible. DeployUS exectue ces scripts en appelant la commande
docker-compose up sur le script de l'usager.

Pour la machine localhost, DeployUS utilise les ports 5000 pour l'interface
client. La base de données MySQL utilise le port 3306 pour toute communication.

### Base de données
La base de données est un service docker avec comme image MySQL. Elle contient 3 tables.
1. scripts
    * (**id**, name, cre_date, contents)
    * id: Identifiant unique du script
    * name: Nom unique du script
    * cre_date: Date du téléversement du script.
    * contents: Le contenu du script.
2. workers
    * (**id**, name, location)
    * id: Identifiant unique du WorkUS
    * name: Nom unique du WorkUS
    * location: Adresse IP unique du WorkUS. 
3. jobs
    * (**id**, script_id, worker_id, launch_date)
    * id: Identifiant unique du WorkUS
    * script_id: Identifiant unique du script en exécution
    * worker_id: Identifiant unique du WorkUS qui execute le script
    * launch_date: Date de lancement du script

### REST Endpoints
* **/**
    * Méthode: GET
    * Page d'entrée du client. Permet d'ajouter et de retirer des scripts.
* **/get_scripts**
    * Méthode: GET
    * Renvoie un JSON du contenue de la table *scripts* de la base de données.
* **/get_workers**
    * Méthode: GET
    * Renvoie un JSON du contenue de la table *workers* de la base de données.
* **/get_jobs**
    * Méthode: GET
    * Renvoie un JSON du contenue de la table *jobs* de la base de données.
* **/insert_script**
    * Méthode: POST
    * Doit envoyer un fichier nommé *scriptfile*
    * Doit envoyer le nom *name* en format multiform.
    * Insère le script dans la base de donner si possible. Renvoie le code 200 si c'est un succès, 422 si un échec.
* **/delete_script**
    * Méthode: POST
    * Doit envoyer un json avec comme format \{"id": \<script_id\>\}
    * Supprime le script ayant id=\<script_id\> de la base de données.
    * Retourne 422 si une erreur s'est produite, 200 aussi non.
* **/launch**
    * Méthode: GET
    * Page du client qui montre les script actifs. Permet de déployer des scripts sur des WorkUS distants.
* **/launch_job**
    * Méthode: POST
    * Nécessite un json avec \{"id"=<script_id>, "location"=<worker_id>\}
    * Tente de déployer le script ayant id=script_id au WorkUS ayant le id=worker_id.
    * Retourne le code 422 si une érreur s'est produite, sinon 200.
* **/stop_job**
    * Méthode: POST
    * Nécessite un json avec \{"id"=<job_id>\}
    * Tente de d'arreter le déployement avec id=<job_id>
    * Retourne le code 422 si une érreur s'est produite, sinon 200.
* **/workers**
    * Méthode: GET
    * Page du client qui montre les WorkUS de la base des données. Permet d'ajouter des WorkUS distants.
* **/insert_workers**
    * Méthode: POST
    * Nécessite un json avec \{"name"=<worker_name>, "location"=<worker_ip_addr>\}
    * Insere un nouveau WorkUS dans la base de données.
    * Retourne 422 si une érreur s'est produite, sinon 200.
* **/delete_worker**
    * Méthode: POST
    * Nécessite un json avec \{"id"=<worker_id>\}
    * Suprimme un WorkUS de la base de données.
    * Retourne 422 si une erreur s'est produite, 200 aussi non.

### Exécution
Ces commandes ont été tester dans un environement unix. 

Pour executer DeployUS:
```
cd DeployUS
docker-compose up
```

Pour se connecter au client graphique, ouvrir un navigateur web et aller à l'address:
```
http://localhost:5000/
```

Évidement, si le micro-service *DeployUS* s'execute sur une machine distante, on doit
utiliser l'address ip de la machine au lieu de localhost. 

### Tests
Ces tests ont été exécuté avec succès sous un environement docker:
```
docker run -it -v $(pwd):/TP3 -v "/var/run/docker.sock:/var/run/docker.sock" docker:20.10

# Dans le conteneur docker
apk add --no-cache docker-compose python3
cd TP3
python3 run_tests.py -t DeployUS
```

Pour executer les tests sans être dans un conteneur docker, vous devez installer python3. il se peut que vous devez 
ajouter les privileges d'administrateurs.
```
sudo python3 run_tests.py -t DeployUS
```

### Dépendances Extérieures
J'utilise le script **wait-for-it.sh** de **https://github.com/vishnubob/wait-for-it/** pour que mes services
attendent que leurs services dont ils dépendent soit fonctionnels. Sa license est la MIT license. Selon mes 
recherches, ce script est régulièrement utilisé dans la communauté Docker. 

Pour faire fonctionner mes tests sur Gitlab CI, j'ai suivi ce tutoriel suivant:  
**https://bohumirzamecnik.cz/blog/2018/gitlab-docker-compose-tests/**.  
J'ai repris de son code et je l'ai modifié pour ma situation. Voir le fichier 
run_tests.py

Pour le linting, j'utilise les images de cytopia (https://github.com/cytopia). Ce sont des images deja contruites
et qui contiennent le nécessaire.

# Micro-service WatchUS

### Bibliothèques externes
* Le micro-service WatchUS est écrit avec le langage de programmation Go. 
* Pour mes tests, je fait des assertions avec le module assert de testify (https://github.com/stretchr/testify/tree/master/assert). Licence MIT.
* L'analyse de code statique est fait avec golint (https://github.com/cytopia/docker-golint) et gofmt (https://github.com/cytopia/docker-gofmt). Licence MIT.

### Fonctionnement
WatchUS rend publique les endpoints */up* et */down* sur le port 5001. Il à la capacité de recevoir un script docker-compose.yml et le sauvegarde à **/work/docker-compose.yml**.

En éffectuant une requête POST à /up avec un json en format {"file": <contents of docker-compose.yml>}, le WatchUS va sauvegarder le fichier dans son répertoire /work. Le WatchUS va ensuite faire un **docker-compose pull** suivi par 
un **docker-compose up -d --force-recreate**. Des appels sucessives de /up peut servir à un déploiement 
en continue de l'application qu'il regarde.

En éffectuant une requête GET à /down, le WatchUS va faire un simple **docker-compose down**.

### REST Endpoints
* **/up**
    * Méthode: POST
    * Nécessite un json de format {"file": <file_contents>}
    * Sauvegarde le script à /work/docker-compose.yml. Fait un **docker-compose pull** dessus et ensuite un **docker-compose up**.
    * Retourne le code 422 ou 500 si une erreur s'est produite, sinon 200
* **/down**
    * Méthode: GET
    * Fait un **docker-compose down** sur le script qu'il regarde. Utile pour les tests.
    * Retourne le code 422 ou 500 si une erreur s'est produite, sinon 200

### Exécution
Ces commandes ont été tester dans un environement unix. 

Pour executer WatchUS:
```
cd WatchUS
docker-compose up
```
On peut ensuite faire des requêtes vers /up et /down. 
```
http://localhost:5001/up
http://localhost:5001/down

```
Évidement, si le micro-service *WatchUS* s'execute sur une machine distante, on doit
utiliser l'address ip de la machine au lieu de localhost. 

### Tests
Ces tests ont été exécuté avec succès sous un environement docker:
```
docker run -it -v $(pwd):/TP3 -v "/var/run/docker.sock:/var/run/docker.sock" docker:20.10

# Dans le conteneur docker
apk add --no-cache docker-compose python3
cd TP3
python3 run_tests.py -t WatchUS
```

Pour executer les tests sans être dans un conteneur docker, vous devez installer python3. il se peut que vous devez 
ajouter les privileges d'administrateurs.
```
sudo python3 run_tests.py -t WatchUS
```

### Dépendances Externes

Pour faire fonctionner mes tests sur Gitlab CI, j'ai suivi ce tutoriel suivant:  
**https://bohumirzamecnik.cz/blog/2018/gitlab-docker-compose-tests/**.  
J'ai repris de son code et je l'ai modifié pour ma situation. Voir le fichier run_tests.py.

Pour l'analyse de code statique, j'utilise les images de cytopia (https://github.com/cytopia). Ce sont des images deja contruites
et qui contiennent le nécessaire.

# Micro-service WorkUS

### Bibliothèques externes
* Le micro-service WorkUS est écrit avec le langage de programmation Go. 
* Pour mes tests, je fait des assertions avec le module assert de testify (https://github.com/stretchr/testify/tree/master/assert). Licence MIT.
* L'analyse de code statique est fait avec golint (https://github.com/cytopia/docker-golint) et gofmt (https://github.com/cytopia/docker-gofmt). Licence MIT.

### Fonctionnement
WorkUS rend publique les endpoints */up* et */down* sur le port 5002. Il à la capacité de recevoir un script docker-compose.yml et le sauvegarde à **/work/scripts/<script_name>/docker-compose.yml**.

En éffectuant une requête POST à /up avec un json en format {"name":<script_name>, "file": <contents of docker-compose.yml>}, le WorkUS va sauvegarder le fichier dans son répertoire /work/scripts/<script_name>/. Le WatchUS va ensuite faire un **docker-compose pull** suivi par 
un **docker-compose up -d --force-recreate**. Des appels sucessives de /up peut servir à un déploiement 
en continue de l'application qu'il regarde.

En éffectuant une requête GET à /down, le WatchUS va faire un simple **docker-compose down**.

### REST Endpoints
* **/up**
    * Méthode: POST
    * Nécessite un json de format {"name":<script_name>, "file": <contents of docker-compose.yml>}
    * Sauvegarde le script à /work/docker-compose.yml. Fait un **docker-compose pull** dessus et ensuite un **docker-compose up**.
    * Retourne le code 422 ou 500 si une erreur s'est produite, sinon 200
* **/down**
    * Méthode: POST
    * Nécessite un json de format {"name":<script_name>}
    * Fait un **docker-compose down** sur le script qu'il regarde. Utile pour les tests.
    * Retourne le code 422 ou 500 si une erreur s'est produite, sinon 200

### Exécution
Ces commandes ont été tester dans un environement unix. 

Pour executer WorkUS:
```
cd WorkUS
docker-compose up
```
On peut ensuite faire des requêtes vers /up et /down. 
```
http://localhost:5002/up
http://localhost:5002/down

```
Évidement, si le micro-service *WorkUS* s'execute sur une machine distante, on doit
utiliser l'address ip de la machine au lieu de localhost. 

### Tests
Ces tests ont été exécuté avec succès sous un environement docker:
```
docker run -it -v $(pwd):/TP3 -v "/var/run/docker.sock:/var/run/docker.sock" docker:20.10

# Dans le conteneur docker
apk add --no-cache docker-compose python3
cd TP3
python3 run_tests.py -t WorkUS
```

Pour executer les tests sans être dans un conteneur docker, vous devez installer python3. il se peut que vous devez 
ajouter les privileges d'administrateurs.
```
sudo python3 run_tests.py -t WorkUS
```

### Dépendances Externes

Pour faire fonctionner mes tests sur Gitlab CI, j'ai suivi ce tutoriel suivant:  
**https://bohumirzamecnik.cz/blog/2018/gitlab-docker-compose-tests/**.  
J'ai repris de son code et je l'ai modifié pour ma situation. Voir le fichier run_tests.py.

Pour l'analyse de code statique, j'utilise les images de cytopia (https://github.com/cytopia). Ce sont des images deja contruites
et qui contiennent le nécessaire.

