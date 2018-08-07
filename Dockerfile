FROM rootproject/root-ubuntu16

# Run the following commands as super user (root):
USER root

# Create a user that does not have root privileges 
ARG username=tmitchel
RUN userdel builder && useradd --create-home --home-dir /home/${username} ${username}
ENV HOME /home/${username}

WORKDIR /ana
ADD . /ana

#EXPOSE 80

ENV ifile ZZ4l

RUN ./build Analyzer
CMD ["sh", "-c", "./Analyzer ${ifile}"]
