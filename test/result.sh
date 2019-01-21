#!/bin/bash
a=$1
while [ $a -gt 0 ]
do
curl -o a.txt 127.0.0.1:8080/1.mp3
curl -o a.txt 127.0.0.1:8080/2.html
curl -o a.txt 127.0.0.1:8080/a.jpg
curl -o a.txt 127.0.0.1:8080/movie.mp4
((a--))
done
