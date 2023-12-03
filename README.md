
/*! \addtogroup installation

# étape 1 : obtenir les sources

créer un répertoire de travail, puis :

    git clone https://forge.univ-lyon1.fr/p1908367/mmv.git

# étape 2 : installer les dépendances

## linux

installez les paquets, si nécessaire (pas la peine au nautibus) : `libsdl2-dev, libsdl2-image-dev, libglew-dev` et `premake4`, ou premke5 s'il est disponible.
par exemple, pour ubuntu et ses variantes :

    sudo apt install libsdl2-dev libsdl2-image-dev libglew-dev premake4

# étape 3 : générer les projets

## windows + visual studio 

pour générer une solution (groupe de projets) visual studio, il suffit de choisir la bonne version :

\code
./premake5.exe vs2019

la solution visual studio ainsi que les projets sont crées dans le répertoire `build/`, ouvrez `build/gkit2light.sln`.

## linux + makefile

\code
premake4 gmake		// si premake4 est installe dans le système
./premake/premake5.linux gmake	// si premake5 est copie dans le repertoire de gKit
\endcode

le `Makefile` se trouve dans le répertoire de base de gKit. 

_remarque :_ si premake5 est disponible dans les paquets de votre distribution utilisez-le ! cf `premake5 gmake`

## utilisation des makefiles

les makefile peuvent générer les versions debug (cf utiliser un debugger comme gdb ou lldb) ou les versions release, plus rapide (2 ou 3 fois, 
interressant pour les projets avec beaucoup de calculs) :
    - `make help`, affiche la liste des projets et les options disponibles,
    - `make tuto7_camera`, compile la version debug de tuto7_camera,
    - `make tuto7_camera config=release`, compile la version release, 64bits, de tuto7_camera,
    - `make tuto7_camera config=debug`, compile la version debug, 64bits, de tuto7_camera,
    - `make tuto7_camera verbose=1`, compile la version debug de tuto7_camera et affiche le détail des commandes exécutées.

les exécutables sont crées dans le répertoire `gkit2light/bin`, pour les exécuter : 

    bin/tuto7_camera
    ou
    bin/base

__remarque :__ gKit charge quelques fichiers au démarrage, il faut l'exécuter depuis le répertoire de base `gKit2light/`, sinon les fichiers ne seront pas correctement
chargés.