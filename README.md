# EQ02

Par: Shawn Vosburg (voss2502)  
Ce qui est accomplie par l'auteur:
Tout le travail demandé.

## YAML
Le YAML Parser se trouve ici: https://github.com/shawnvosburg/yaml-cpp.  
C'est un simple *fork* de: https://github.com/jbeder/yaml-cpp.


## Les Caches
Pour un recompilation minimale, j'utilise un fichier compile.cache qui se trouve dans le dossier intermediare. Le format de ce fichier est:
```
<nom du fichier compilé>\0<nom du fichier source>\0<SHA1 du contenu du fichier source>
...
```
Chaque ligne représente un fichier compilé. 


Pour savoir où est l'executable (e.g. pour le supprimer), une cache spéciale qui se nomme project.cache se trouve dans le dossier intermediate. Le format de ce fichier est:
```
<Emplacement de l'executable relatif au dossier parent>
...


