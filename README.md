# DeployUS
Par: Shawn Vosburg (voss2502)  
Équipe: EQ02  
Email: shawn.vosburg@[usherbrooke.ca, mail.mcgill.ca]  
Date: 12 avril 2021  
<br>
Dans le cadre du cours IGL711 de l'Université de Sherbrooke.
Partie réalisé jusqu'à présent: Part0

### Introduction
DeployUS est une agglomération de microservice qui performe le déploiement
d'applications *docker-compose* sur des machines distantes via REST API. Un client
graphique a été développé pour en facilité l'utilisation.   

DeployUS est conçu pour être installer sur des serveurs distant et être accèdé 
via un navigateur web. Cet outil est idéal pour le déploiement d'application web.

DeployUS est écrit entièrement en Python.

### Dépendances
Pour executer DeployUS, la machine hôte doit avoir ces programes suivant.
1. docker
2. docker-compose
3. python3 (testé avec python3.8 mais peut possiblement fonctionner avec des versions antérieures)

# Micro-service DeployUS

## Bibliothèques externes
* DeployUS est ecrit avec la bilbiotèque Flask, disponible sur PyPI.
* Pour connecter avec la base de données MySQL, j'utilise la librairie mysql-connector de PyPI.
* Pour faire de requêtes REST dans mes tests, j'utilise la librairie requests de PyPI.

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
client et le port 32000 pour la communication avec la base de données. Ces ports sont
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

## Tests
Ces tests ont été exécuté avec succès sous un environement docker:
```
sudo docker run -it -v $(pwd):/TP3 -v "/var/run/docker.sock:/var/run/docker.sock" docker:20.10

# Dans le conteneur docker
apk add docker-compose python3
cd TP3/DeployUS
python3 tests/run_tests.py
```

Pour executer les tests sans être dans un conteneur docker, il se peut que vous devez 
ajouter les privileges d'administrateurs.
```
cd DeployUS
sudo python3 tests/run_tests.py
```

## Remerciement
J'utilise le script **wait-for-it.sh** de **https://github.com/vishnubob/wait-for-it/** pour que mes services
attendent que leurs services dont ils dépendent soit fonctionnels. Sa license est la MIT license. Selon mes 
recherches, ce script est régulièrement utilisé dans la communauté Docker. 

Pour faire fonctionner mes tests sur Gitlab CI, j'ai suivi ce tutoriel suivant:  
**https://bohumirzamecnik.cz/blog/2018/gitlab-docker-compose-tests/**.  
J'ai repris de son code et je l'ai modifié pour ma situation. Voir le fichier 
DeployUS/tests/run_tests.py
