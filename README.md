# DeployUS
Par: Shawn Vosburg (voss2502)  
Équipe: EQ02  
Email: shawn.vosburg@[usherbrooke.ca, mail.mcgill.ca]  
Date: 12 avril 2021  
<br>
Dans le cadre du cours IGL711 de l'Université de Sherbrooke.  
Partie réalisé jusqu'à présent: [Part0, Part1]

### Introduction
DeployUS est une agglomération de microservice qui performe le déploiement
d'applications *docker-compose* sur des machines distantes via REST API. Un client
graphique a été développé pour en facilité l'utilisation.   

DeployUS est conçu pour être installer sur des serveurs distant et être accèdé 
via un navigateur web. Cet outil est idéal pour le déploiement d'application web.

Le micro-service DeployUS est écrit entièrement en Python.  
Le micro-service WatchUS est écrit GO avec un script de lancement des tests en Python.  


### Dépendances
Pour executer DeployUS, la machine hôte doit avoir ces programes suivant.
1. docker
2. docker-compose

# Micro-service DeployUS

## Bibliothèques externes
* Le micro-service DeployUS est ecrit avec la bilbiotèque Flask, disponible sur PyPI.
* Pour connecter avec la base de données MySQL, j'utilise la librairie mysql-connector de PyPI.
* Pour executer les tests, j'utilise pytest de PyPi.
* Pour faire de requêtes REST dans mes tests, j'utilise la librairie requests de PyPI.
* L'analyse de code statique est fait à l'aide des modules pylint (https://github.com/cytopia/docker-pylint) et pycodestyle (https://github.com/cytopia/docker-pycodestyle) de PyPI.

## Fonctionnement
DeployUS utilise des requêtes REST pour recevoir des scripts valide de docker-compose.yml.
Ces scripts doivent:
* avoir un nom qui fini par .yml
* être valid selon « docker-compose config »

L'utilisateur peut ensuite déclarer des machines distantes comme « worker » et 
déployer leur script sur ces machines. Présentement, seulment la machine locale
(i.e. localhost) est disponible. DeployUS exectue ces scripts en appelant la commande
docker-compose up sur le script de l'usager.

Pour la machine localhost, DeployUS utilise les ports 5000 pour l'interface
client et le port 3306 pour la communication avec la base de données. Ces ports sont
alors réserver et ne peuvent être utilisé par l'application d'un usager.

## Execution
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

## Tests
Ces tests ont été exécuté avec succès sous un environement docker:
```
sudo docker run -it -v $(pwd):/TP3 -v "/var/run/docker.sock:/var/run/docker.sock" docker:20.10

# Dans le conteneur docker
apk add --no-cache docker-compose python3
cd TP3/DeployUS/tests
python3 run_tests.py
```

Pour executer les tests sans être dans un conteneur docker, vous devez installer python3. il se peut que vous devez 
ajouter les privileges d'administrateurs.
```
cd DeployUS/tests
sudo python3 run_tests.py
```

## Remerciement
J'utilise le script **wait-for-it.sh** de **https://github.com/vishnubob/wait-for-it/** pour que mes services
attendent que leurs services dont ils dépendent soit fonctionnels. Sa license est la MIT license. Selon mes 
recherches, ce script est régulièrement utilisé dans la communauté Docker. 

Pour faire fonctionner mes tests sur Gitlab CI, j'ai suivi ce tutoriel suivant:  
**https://bohumirzamecnik.cz/blog/2018/gitlab-docker-compose-tests/**.  
J'ai repris de son code et je l'ai modifié pour ma situation. Voir le fichier 
DeployUS/tests/run_tests.py

Pour le linting, j'utilise les images de cytopia (https://github.com/cytopia). Ce sont des images deja contruites
et qui contiennent le nécessaire.

# Micro-service WatchUS

## Bibliothèques externes
* Le micro-service WatchUS est écrit avec le langage de programmation Go. 
* Pour mes tests, je fait des requêtes REST avec la bibliothèque Resty (https://github.com/go-resty/resty)
* Pour mes tests, je fait des assertions avec le module assert de testify (https://github.com/stretchr/testify/tree/master/assert)
* L'analyse de code statique est fait avec golint (https://github.com/cytopia/docker-golint) et gofmt (https://github.com/cytopia/docker-gofmt)

## Fonctionnement
WatchUS rend publique les endpoints */up* et */down* sur le port 5001. Il assume 
aussi que l'application qu'il suit se trouve à **/work/docker-compose.yml**.

En éffectuant une requête GET à /up, le WatchUS va faire un **docker-compose pull** suivi par 
un **docker-compose up -d --force-recreate**. Des appels sucessives de /up peut servir à un déploiement 
en continue de l'application qu'il regarde.

En éffectuant une requête GET à /down, le WatchUS va faire un simple **docker-compose down**.

## Execution
Ces commandes ont été tester dans un environement unix. 

Pour executer WatchUS:
```
cd WatchUS
docker-compose up
```
On peut ensuite faire des requêtes vers /up et /down sur un navigateur web.
```
http://localhost:5001/up
http://localhost:5001/down

```
Évidement, si le micro-service *WatchUS* s'execute sur une machine distante, on doit
utiliser l'address ip de la machine au lieu de localhost. 

## Tests
Ces tests ont été exécuté avec succès sous un environement docker:
```
sudo docker run -it -v $(pwd):/TP3 -v "/var/run/docker.sock:/var/run/docker.sock" docker:20.10

# Dans le conteneur docker
apk add --no-cache docker-compose python3
cd TP3/WatchUS/tests
python3 run_tests.py
```

Pour executer les tests sans être dans un conteneur docker, vous devez installer python3. il se peut que vous devez 
ajouter les privileges d'administrateurs.
```
cd DeployUS/tests
sudo python3 run_tests.py
```

## Remerciement

Pour faire fonctionner mes tests sur Gitlab CI, j'ai suivi ce tutoriel suivant:  
**https://bohumirzamecnik.cz/blog/2018/gitlab-docker-compose-tests/**.  
J'ai repris de son code et je l'ai modifié pour ma situation. Voir le fichier 
WatchUS/tests/run_tests.py

Pour l'analyse de code statique, j'utilise les images de cytopia (https://github.com/cytopia). Ce sont des images deja contruites
et qui contiennent le nécessaire.

