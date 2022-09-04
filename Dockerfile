FROM wiiuenv/devkitppc:20220806

COPY --from=wiiuenv/libkernel:20220904 /artifacts $DEVKITPRO
COPY --from=wiiuenv/wiiumodulesystem:20220904 /artifacts $DEVKITPRO
COPY --from=wiiuenv/libmocha:20220903 /artifacts $DEVKITPRO

WORKDIR project