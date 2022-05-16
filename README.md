![alt text](https://gitlab.com/ouzb64ty/supstream/-/raw/main/images/supstream-logo.png)

*Superviseur de stream basé sur Gstreamer et configurable en YAML.* - tpoac <t.poac@cyim.com>

# Introduction

Supstream est un projet open-source de déploiement et de developpement de pipelines de streaming dynamique. Ce projet utilise le format YAML pour configurer les differentes pipelines pour un usage aise entres plusieurs parties d'une architecture. Il est un outil «dev-ops» pour la mise en place de pipeline de streaming, épargnant un temps de développement conséquent, Supstream permet de s'affranchire du changement des versions de Gstreamer dans un contexte sécurisé et conteneurisé sous Linux. La particularitée de Supstream est de pouvoir centraliser un ensembles de flux, ce qui n’est pas négligeable dans le cas d'une architecture microservice pouvant nécéssiter d‘une supervision, d’un monitoring mais encore d’une gestion approfondie des streams. Supstream intégre une API REST (APIGateway) en Python/uvicorn utilisant APIFast, pour le moment c'est un POC. En général, ce projet est une «Virtual Machine» éxécutant un ensemble d’opérations censées uniformiser la mise en place d’architecture de streaming audio & vidéo. A l’heure actuelle, aucuns projets open-source comme celui ci n’existe. En revanche il existe un autre projet open-source légèrement similaire à Supstream nommé «gst-d» pour Gstreamer daemon, développé par la société RidgeRun qui fait du support Gstreamer. Il faut savoir que ce projet intégre des pipelines entrantes sous le format CLI de Gstreamer, «gst-launch», à contrario, Supstream utilise directement les fonctions bas niveau de la librairie Gstreamer permettant d’élargir et d’améliorer nativement le control sur chaqu’unes d’elles.

L’éxécutable Supstream est aussi rapide que la commande «gst-launch», en l’occurrence elle utilise du YAML et lance un serveur d’écoute pour la supervision et le monitoring. Techniquement, le parsing de ce projet est relativement bien développé à partir d'une analyse lexicale, syntaxicale et sémantique en utilisant des arbres binaires abstraits pour toutes les parties en rapport avec l’interprétation, par exemple des configurations. Des tables de hash sont notamment intégrées dans ce projet pour permettre un partage de certaines données Gstreamer.

**Supstream pour SUPervisor STREAM**, intégre :
- La gestion des caps
- L'attribution des propriétés
- Le linkage des pads et des éléments
- Un repertoire de 30 samples pré-configurés
- une supervision des pipelines par ordre de priorité
- une supervision des pipelines par type d'éxécution (synchronisé/threadé)
- la sauvegarde des logs dans un fichier
- l'initialisation de l'état des pipelines
- la génération des graphs pour visualiser les pipelines lancées par Gstreamer
- la gestion des à travers des schedulers et des delays
- l'édition de la timezone
- un docker qui lance une compilation Gstreamer via Cerbero et Ninja, uniquement dans un environnement de développement pour le moment
- l'auto restart des pipelines
- Un "DOM" chargé en mémoire et mis à jour lors de chaques modifications
- l'activation d'une API IPC pour modifier en temps-réel les pipelines

l'API permet en temps réel *(🚧 En cours de développement)* :
- Le control des états en temps-réel des pipelines (play, pause, null et ready)
- L'obtention de la version
- L'obtention de l'actuel "DOM" contenant les éléments, les pipelines, la configuration..
- Le relinkage des éléments
- La création et la suppression des éléments
- La modification des propriétés d'éléments et de pads
- La modification des caps
- La fermeture du programme Supstream

Il faut utiliser le conteneur docker pour le moment, cela peut durer quelques heures car il compile toutes les sources Gstreamer depuis leurs sources.
Actuellement, le projet est en cours de développement. Pour mieux comprendre l'intérêt de ce projet, vous pouvez jeter un oeil sur le dossier _samples_ du répertoire.
Un trello est accessible à l'adresse : https://trello.com/b/gAa0tKvO/supstream, il contient l'ensembles des tâches courantes, c.à.d, en cours de développement.

![alt text](https://gitlab.com/ouzb64ty/supstream/-/raw/main/images/screenshot.png)

# Installation

⚠️ Il est préférable de builder le projet depuis son Docker ou d'installer la dernière version de Gstreamer, Supstream est censé s'affranchir du problème de version en installant automatiquement l'une des dernières version de Gstreamer.

## Compilation
### Ubuntu 21.10

1. Vous devez installer Gstreamer via Cerbero (https://gstreamer.freedesktop.org/documentation/installing/index.html?gi-language=c)
2. `apt install libyaml-dev && apt-install libczmq-dev && libcjson-dev`
3. `git clone https://gitlab.com/ouzb64ty/supstream.git supstream && cd supstream && make`

## Docker

Ce projet contient un Dockerfile pour un mode de développement simple temporairement basé sur un conteneur ubuntu:latest.

- Installation des dépendances Gstreamer
- Compilation Gstreamer avec meson et ninja
- Compilation des plugins Gstreamer
- Configuration VIM et ZSH
- Installation des dépendances de l'API Gateway

1. Builder le Docker Supstream

```bash
docker build . -t docker-gstreamer-compiler
```

2. Faire un export display

```bash
xhost +
```

3. Lancer un docker Supstream

```bash
docker run --ipc=host -v=/tmp:/tmp --rm -it --device /dev/video0 --net host -e DISPLAY=$DISPLAY --device /dev/snd supstream
```

Pour information, _--device /dev/video0_ permet l'utilisation de l'élément v4l2src pour la webcam, tandis que _--device /dev/snd_ permet l'utilisation de l'audio.

# Running

- Just type `./bin/supstream -f samples/matroska_video_0.yaml`

```yaml
pipelines:

    matroska_video_0:

        init_state: play
        type_exec: sync

        elements:
            souphttpsrc:
                element: souphttpsrc
                properties:
                    location: https://www.freedesktop.org/software/gstreamer-sdk/data/media/sintel_trailer-480p.webm
                element_link: matroskademux
            matroskademux:
                element: matroskademux
                pad_link:
                    pad_video:
                        from: video_0
                        to:
                            name: vp8dec
                            pad: sink
            vp8dec:
                element: vp8dec
                element_link: videoconvert
            videoconvert:
                element: videoconvert
                element_link: autovideosink
            autovideosink:
                element: autovideosink
```
This configuration allows you to easily display a WEBM video from an MKV demuxer

- Just type `./bin/supstream -f samples/mosaic.yaml`

```yaml
##############
# GST LAUNCH #
##############

# gst-launch-1.0 -e videomixer name=mix \                                               
# sink_0::xpos=0 sink_0::ypos=0 \
# sink_1::xpos=10 sink_1::ypos=10 \
# sink_2::xpos=320 sink_2::ypos=10 \
# sink_3::xpos=10 sink_3::ypos=180 \
# sink_4::xpos=320 sink_4::ypos=180 \
# ! videoconvert ! xvimagesink \
# videotestsrc pattern=snow ! video/x-raw,width=650,height=370 ! mix.sink_0 \
# videotestsrc ! videoconvert ! videoscale ! video/x-raw,width=320,height=180 ! mix.sink_1 \
# videotestsrc ! videoconvert ! videoscale ! video/x-raw,width=320,height=180 ! mix.sink_2 \
# videotestsrc ! videoconvert ! videoscale ! video/x-raw,width=320,height=180 ! mix.sink_3 \
# videotestsrc ! videoconvert ! videoscale ! video/x-raw,width=320,height=180 ! mix.sink_4

##########
# CONFIG #
##########

supstream:
    zmq_path: "tcp://0.0.0.0:5557"
    timezone: "Europe/Paris"

#############
# PIPELINES #
#############

pipelines:

    ##################
    # VIDEO PIPELINE #
    ##################

    mosaic_video:

        init_state: play
        type_exec: sync

        elements:

            #########
            # MIXER #
            #########

            videomixer:

                element: videomixer
                element_link: videoconvert
                pad_props:

                    ##############
                    # 4/4 MOSAIC #
                    ##############

                    panel:
                        pad: sink_0
                        properties:
                            xpos: 0
                            ypos: 0

                    top_left:
                        pad: sink_1
                        properties:
                            xpos: 10
                            ypos: 10

                    top_right:
                        pad: sink_2
                        properties:
                            xpos: 320
                            ypos: 10

                    bottom_left:
                        pad: sink_3
                        properties:
                            xpos: 10
                            ypos: 180

                    bottom_right:
                        pad: sink_4
                        properties:
                            xpos: 320
                            ypos: 180


            videoconvert:

                element: videoconvert
                element_link: xvimagesink

            xvimagesink:
                
                element: xvimagesink

            #########
            # PANEL #
            #########

            videotestsrc_panel:

                element: videotestsrc
                properties:
                    pattern: 1
                caps:
                    media_type: video/x-raw
                    width: 650
                    height: 370
                element_link: videoconvert_panel

            videoconvert_panel:

                element: videoconvert
                pad_link:

                    link_panel:
                        from: src
                        to:
                            name: videomixer
                            pad: sink_0



            ################
            # TOP LEFT 1/4 #
            ################
            
            videotestsrc_top_left:

                element: videotestsrc
                properties:
                    pattern: 0
                element_link: videoconvert_top_left

            videoconvert_top_left:

                element: videoconvert
                element_link: videoscale_top_left
                caps:
                    media_type: video/x-raw
                    width: 320
                    height: 180
                    framerate: "30000/1001"

            videoscale_top_left:

                element: videoscale
                pad_link:

                    link_top_left:
                        from: src
                        to:
                            name: videomixer
                            pad: sink_1


            #################
            # TOP RIGHT 2/4 #
            #################

            videotestsrc_top_right:

                element: videotestsrc
                properties:
                    pattern: 0
                element_link: videoconvert_top_right

            videoconvert_top_right:

                element: videoconvert
                element_link: videoscale_top_right
                caps:
                    media_type: video/x-raw
                    width: 320
                    height: 180

            videoscale_top_right:

                element: videoscale
                pad_link:

                    link_top_right:
                        from: src
                        to:
                            name: videomixer
                            pad: sink_2


            ###################
            # BOTTOM LEFT 3/4 #
            ###################

            videotestsrc_bottom_left:

                element: videotestsrc
                element_link: videoconvert_bottom_left
                properties:
                    pattern: 0

            videoconvert_bottom_left:

                element: videoconvert
                element_link: videoscale_bottom_left
                caps:
                    media_type: video/x-raw
                    width: 320
                    height: 180

            videoscale_bottom_left:

                element: videoscale
                pad_link:

                    link_bottom_left:
                        from: src
                        to:
                            name: videomixer
                            pad: sink_3


            ####################
            # BOTTOM RIGHT 4/4 #
            ####################

            videotestsrc_bottom_right:

                element: videotestsrc
                element_link: videoconvert_bottom_right
                properties:
                    pattern: 0

            videoconvert_bottom_right:

                element: videoconvert
                element_link: videoscale_bottom_right
                caps:
                    media_type: video/x-raw
                    width: 320
                    height: 180

            videoscale_bottom_right:

                element: videoscale
                pad_link:

                    link_bottom_right:
                        from: src
                        to:
                            name: videomixer
                            pad: sink_4

```

This configuration allows you to easily display a mosaic

- Just type `./bin/supstream -f samples/gstdarknet_yolov4.yaml`

```yaml
supstream:
  zmq_path: "tcp://0.0.0.0:5557"
  timezone: "Europe/Paris"

pipelines:
    
  road_detection:

    init_state: play
    type_exec: thread
    set_delay: "2022-04-19 12:17:20"

    elements:
      
      filesrc:
        element: filesrc
        properties:
          location: /tmp/test.mp4
        element_link: qtdemux

      qtdemux:
        element: qtdemux
        caps:
          media_type: video/x-h264
        pad_link:
          pad_video_3:
            from: video_0
            to:
              name: h264parse
              pad: sink

      h264parse:
        element: h264parse
        element_link: avdec_h264

      avdec_h264:
        element: avdec_h264
        element_link: videoconvert

      videoconvert:
        element: videoconvert
        element_link: darknetinfer
    
      darknetinfer:
        element: darknetinfer
        properties:
          config: /tmp/yolov4.cfg
          weights: /tmp/yolov4.weights
        element_link: darknetrender

      darknetrender:
        element: darknetrender
        properties:
          labels: /tmp/coco.names
        element_link: videoconvert3

      videoconvert3:
        element: videoconvert
        element_link: autovideosink

      autovideosink:
        element: autovideosink
```

This configuration allows to apply an object detection algorithm from a video stream using darknet YOLOv4 and a set-delay for broadcasting

# ZeroMQ API Documentation

This textual documentation describes how _supstream_ externally talk with IPC ZeroMQ

- codes :
    - 0 success
    - 1 failure

The default ZMQ uri for IPC/TCP sharing is _ipc:///tmp/supstream.pipe_, you can update your own ZMQ path with _zmq_path_ property.

### Play

Update state to PLAYING for a specific pipeline or rather all

```json
{
    "command": "play",
    "pipelines": ["matroska-video_0"]
}
```

### Pause

Update state to PAUSED for a specific pipeline or rather all

```json
{
    "command": "pause",
    "pipelines": ["matroska-video_0"]
}
```

### Ready

Update state to READY for a specific pipeline or rather all

```json
{
    "command": "ready",
    "pipelines": ["matroska-video_0"]
}
```

### Null

Update state to NULL for a specific pipeline or rather all

```json
{
    "command": "null",
    "pipelines": ["matroska-video_0"]
}
```

### Version

```json
{
    "command": "version"
}
```

### Exit

This request exit a supstream process

```json
{
    "command": "exit"
}
```

### Show

Used to get general pipelines configuration, saved in cache

```json
{
    "command": "show"
}
```

### Unlink element

```json
{
    "command": "unlink_element",
    "pipeline": "matroska-video_0",
    "src": "videoconvert",
    "sink": "autovideosink"
}
```

### Link element

```json
{
    "command": "link_element",
    "pipeline": "matroska-video_0",
    "src": "videoconvert",
    "sink": "autovideosink"
}
```

### Create element

```json
{
    "command": "create_element",
    "pipeline": "matroska-video_0",
    "element": {
    }
}
```

### Set properties

```json
{
    "command": "set_properties",
    "pipeline": "matroska-video_0",
    "element": "videoconvert",
    "properties": {
    }
}
```

### Set pad properties

```json
{
    "command": "set_properties",
    "pipeline": "matroska-video_0",
    "element": "videoconvert",
    "pad": "video_0",
    "properties": {
    }
}
```

### Set caps

```json
{
    "command": "set_caps",
    "pipeline": "matroska-video_0",
    "element": "videoconvert",
    "caps": {
    }
}
```

# Autor

tpoac <t.poac@cyim.com>
