FROM wiiuenv/devkitppc:20210920

COPY --from=wiiuenv/libkernel:20211031 /artifacts $DEVKITPRO
COPY --from=wiiuenv/wiiumodulesystem:20211207 /artifacts $DEVKITPRO

WORKDIR project