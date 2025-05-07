FROM debian:12.10

RUN apt-get clean
RUN apt-get update --fix-missing
RUN apt-get update
RUN DEBIAN_FRONTEND=noninteractive apt-get install -y locales nano gcc make bash-completion whois time wget curl vim procps htop python3-pip python3.11-venv swig iputils-ping iproute2 python3-pandas telnet git

# LOCALE
RUN sed -i '/es_ES.UTF-8/s/^# //g' /etc/locale.gen && locale-gen
ENV LANG=es_ES.UTF-8
ENV LANGUAGE=es_ES:es
ENV LC_ALL=es_ES.UTF-8

## Install virtual env for Ray
WORKDIR /root
ADD requirements-full.txt /root/requirements-full.txt
ADD requirements.txt /root/requirements.txt
ADD install-venv.sh /root/install-venv.sh
RUN chmod +x /root/install-venv.sh ; /root/install-venv.sh
