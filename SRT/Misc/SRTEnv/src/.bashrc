# Source global definitions
if [ -f /etc/bashrc ]; then
	. /etc/bashrc
fi

alias l='ls'
alias la='ls -a'
alias ll='ls -l'

# Import the nuraghe environment
source $HOME/.nuraghe/nuragherc

# Initialize the Python shell
if [ -f $HOME/.pyrc ]
then
    export PYTHONSTARTUP=$HOME/.pyrc
fi

