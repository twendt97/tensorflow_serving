# You can pick any Debian/Ubuntu-based image. 😊
FROM tensorflow/serving:nightly-devel

# Options for setup script
ARG USERNAME=vscode
ARG USER_UID=1000
ARG USER_GID=$USER_UID

RUN groupadd --gid $USER_GID $USERNAME \
    && useradd --uid $USER_UID --gid $USER_GID -m $USERNAME \
    && echo "alias ll='ls -alF'" >> /home/${USERNAME}/.bashrc \
    && echo "source /usr/share/bash-completion/completions/git" >> /home/${USERNAME}/.bashrc \
    && mkdir -p /home/${USERNAME}/.cache \
    && chown -R ${USER_UID}:${USER_GID} /home/${USERNAME}/.cache

RUN SNIPPET="export PROMPT_COMMAND='history -a' && export HISTFILE=/commandhistory/.bash_history" \
    && mkdir /commandhistory \
    && touch /commandhistory/.bash_history \
    && chown -R $USERNAME /commandhistory \
    && echo "$SNIPPET" >> "/home/$USERNAME/.bashrc"

ENV SHELL /bin/bash