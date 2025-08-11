#!/bin/sh


playerctl -p tidal-hifi metadata > /dev/null
if [ $? -ne 0 ]; then
        exit 1
fi


convert_secs() {
        printf "%d:%02d" "$((10#$1 / 60))" "$((10#$1 % 60))";
}

status=$(playerctl -p tidal-hifi status)
artist=$(playerctl -p tidal-hifi metadata artist)
title=$(playerctl -p tidal-hifi metadata title)

# bug in tidal-hifi with mpris length...
#position_raw=$(playerctl -p tidal-hifi position | sed 's/\./\n/g' | head -n 1)

# "no player could handle this command" ?
# echo "$(playerctl -p tidal-hifi metadata length)"

# UGLY
#length_raw=$(($(playerctl -p tidal-hifi metadata | grep length | sed 's/ /\n/g' | tail -n 1) / 1000000))

#position=$(convert_secs "$position_raw")
#length=$(convert_secs "$length_raw")

echo "$status $artist - $title"
