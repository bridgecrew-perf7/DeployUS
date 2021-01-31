# EQ02

Auteur: Shawn Vosburg  
Travail Réalisé: Les commandes **init, add, commit, checkout**  

# Pour rouler les test
```
cd gitus
mkdir build
cd build
cmake ..
make
cd tests
./gittests
```
Le terminal doit absolument être au niveau de gitus/BUILD_FOLDER/tests/ car il y a des fichiers relatifs qui sont utilisés.   

# Format d'un ficher objet representant un ficher 
```
Filename: \0<filename>\0
Filesize: \0<contentsize>\0
File contents: \0<content>\0

```
Les \0 représentent un charactère nul. Ils servents à délimiter la valeur de son en-tête.

# Format d'un ficher objet representant un arbre
```
<tree/blob>\0<sha1 hash>\0<file/folder name>
<tree/blob>\0<sha1 hash>\0<file/folder name>
...
```
Les \0 représentent un charactère nul. Ils servents à délimiter la valeur de son en-tête.  
Chaque ligne représente un objet gitus.


# Format d'un ficher objet representant un commit
```
tree\0<sha1 hash>
parent\0<sha1 hash> (optional)
Author\0<name>
Message\0<commit message>
Time(UTC)\0<datetime>
```
Les \0 représentent un charactère nul. Ils servent à délimiter la valeur de son en-tête.  

# Format du ficher HEAD
```
<commit sha1 hash>
```

# Format du ficher index
```
<filepath>\0<sha1 hash>
<filepath>\0<sha1 hash>
...
```
Chaque ligne represente un objet gitus.

# La Commande Checkout
Pour déterminer quel est le commit le plus récent, un ficher .git/TOP_COMMIT est créer quand 
un commit, autre que le commit le plus récent, est checked out. Ce nouveau ficher suit le même
format que le fichier .git/HEAD et contient le SHA1 du commit le plus récent. Quand le commit le plus récent
est checked out, ce fichier est effacé. 
