T07: check
	@echo "-------- Sample 7 --------"

	"$(FLASCC)/usr/bin/genfs" ../data vfs
	#cp "$(FLASCC)/usr/share/HTTPBackingStore.as" .
	#cp "$(FLASCC)/usr/share/LSOBackingStore.as" .
	#"$(FLASCC)/usr/bin/genfs" --type=http webvfs testwebvfs
	#mv testwebvfs_manifest.as manifest.as

	$(AS3COMPILERARGS)	-import $(call nativepath,$(FLASCC)/usr/lib/builtin.abc) \
				-import $(call nativepath,$(FLASCC)/usr/lib/playerglobal.abc) \
				-import $(call nativepath,$(FLASCC)/usr/lib/BinaryData.abc) \
				-import $(call nativepath,$(FLASCC)/usr/lib/ISpecialFile.abc) \
				-import $(call nativepath,$(FLASCC)/usr/lib/IBackingStore.abc) \
				-import $(call nativepath,$(FLASCC)/usr/lib/IVFS.abc) \
				-import $(call nativepath,$(FLASCC)/usr/lib/InMemoryBackingStore.abc) \
				-import $(call nativepath,$(FLASCC)/usr/lib/PlayerKernel.abc) \
				vfsRootFSBackingStore.as -outdir . -out VFS

	$(AS3COMPILERARGS)	-import $(call nativepath,$(FLASCC)/usr/lib/builtin.abc) \
				-import $(call nativepath,$(FLASCC)/usr/lib/playerglobal.abc) \
				-import $(call nativepath,$(FLASCC)/usr/lib/BinaryData.abc) \
				-import $(call nativepath,$(FLASCC)/usr/lib/ISpecialFile.abc) \
				-import $(call nativepath,$(FLASCC)/usr/lib/IBackingStore.abc) \
				-import $(call nativepath,$(FLASCC)/usr/lib/IVFS.abc) \
				-import $(call nativepath,$(FLASCC)/usr/lib/InMemoryBackingStore.abc) \
				-import $(call nativepath,$(FLASCC)/usr/lib/PlayerKernel.abc) \
				-import VFS.abc \
				Console.as -outdir . -out Console

	#"$(FLASCC)/usr/bin/g++" $(BASE_CFLAGS) -O3 vfstest.cpp VFS.abc $(FLASCC)/usr/lib/AlcVFSZip.abc -symbol-abc=Console.abc -emit-swf -swf-version=17 -swf-size=800x600 -o vfstest.swf

include ./Makefile.common

clean:
	rm -f *.swf *.bc *.abc testvfs* HTTPBackingStore.as LSOBackingStore.as testwebvfs*
