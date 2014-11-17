# Source global definitions
if [ -f /etc/bashrc ]; then
	. /etc/bashrc
fi

alias l='ls'
alias la='ls -a'
alias ll='ls -l'

# Import the nuraghe environment
source $HOME/.acs/nuragherc

export PYTHONSTARTUP=$HOME/.pyrc

