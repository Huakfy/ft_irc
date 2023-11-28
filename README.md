# ft_irc

nc -C 127.0.0.1 <port> <br>
-C permet de mettre des \r\n à chaque entrée (au lieu de \n), permetant d'utiliser le serveur<br>

besoin de fixé la longueur d'un username + ça doit être dit avec le rpl welcome (en réalité rpl isupport 005 juste après le rpl wlcm)

> CAP LS <br>
> PASS pass <br>
> NICK pass <br>
> USER david david 127.0.0.1 :david <br>

https://modern.ircdocs.horse/

Pour l'envoie de fichier
> /dcc <\br>
> /dcc sent <nickname> <path> <\br>
et pour recevoir
> /dcc get <nickname> [<path>]<\br>