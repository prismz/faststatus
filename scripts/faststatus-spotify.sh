#!/bin/sh

set -e

playerctl -p spotify metadata > /dev/null 2>&1
if [ $? -ne 0 ]; then
        exit 1
fi

convert_secs() {
        printf "%d:%02d" "$((10#$1 / 60))" "$((10#$1 % 60))";
}

status_text=$(playerctl -p spotify status)
status=""
case "$status_text" in
        Playing)
                status="▶️ " ;;
        Paused)
                status="⏸️ " ;;
        *)
                status=""
esac

#artist=$(truncate 16 "$(playerctl -p spotify metadata artist)")
#title=$(truncate 16 "$(playerctl -p spotify metadata title)")
scrolling_metadata=$(cat /tmp/scrolling_metadata)
vol_rounded=$(printf "%0.2f" "$(playerctl -p spotify volume)")
volume=$(echo "scale=0; (100 * $vol_rounded)/1" | bc)

position_raw=$(playerctl -p spotify position | sed 's/\./\n/g' | head -n 1)

# "no player could handle this command" ?
# echo "$(playerctl -p spotify metadata length)"

# UGLY
length_raw=$(($(playerctl -p spotify metadata | grep length | sed 's/ /\n/g' | tail -n 1) / 1000000))

track_progress_percent=$(echo "scale=2; $position_raw / $length_raw" | bc)
track_progress_bar=$(progbar $track_progress_percent 20)

position=$(convert_secs "$position_raw")
length=$(convert_secs "$length_raw")

#echo "$track_progress_bar $status$artist - $title $position/$length ($volume%)"
echo "$status $scrolling_metadata $track_progress_bar (🔊 $volume%)"
