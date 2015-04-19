SSHPORT=2323 // if you wish to use another sshd on another port
SKIP_COMPRESS="--skip-compress=.jpg,.jpeg,.gz,.mp3,.mp4,.m4a"
RSYNCOPTS=rlptgoDHxAXz
# the options make rsync exit cleanly if TCP hangs, as when I shut my laptop lid...
RSYNC_SSH="ssh -i $HOME/.ssh/id_rsa.bkp -p $SSHPORT -o ServerAliveInterval=5 -o ServerAliveCountMax=6 -o TCPKeepAlive=yes"
SOURCE=/path/to/folder/including/ending/slash/
dest=user@destination:./$(hostname -s)
# FULL STORE:
rsync -$RSYNCOPTS --exclude-from=/root/rodin_backup.exclude --progress -e "$RSYNC_SSH" --links --numeric-ids $SKIP_COMPRESS $DELETE -R $SOURCE/ $de
st

# INCREMENTAL STORE:
BACKUP_DIR=.${SOURCE}-incremental/$(date +%Y%m%d-%H%M)
rsync -b$RSYNCOPTS --exclude-from=/root/rodin_backup.exclude -e "$RSYNC_SSH" -i --out-format="%o %n" --links --numeric-ids $SKIP_COMPRESS $DELETE -R -
-backup-dir="$BACKUP_DIR" "$SOURCE/" "$dest" 


## TODO:
rsync -av -h --delete --inplace --no-whole-file --stats /sourcedir /mnt/backup/current 

## SEE ALSO:
#rdiff-backup
#duplicity.nongnu.org
