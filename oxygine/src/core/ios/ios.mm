#import <Foundation/Foundation.h>

#if TARGET_OS_IPHONE
#import <UIKit/UIImage.h>
#else
#import <AppKit/AppKit.h>
#endif

#include "ios.h"
#include "MemoryTexture.h"

namespace oxygine
{
	namespace file
	{
		std::string getSupportFolder()
		{
            NSString* bundleID = [[NSBundle mainBundle] bundleIdentifier];
            NSFileManager*fm = [NSFileManager defaultManager];
            NSURL*    dirPath = nil;
            
            // Find the application support directory in the home directory.
            NSArray* appSupportDir = [fm URLsForDirectory:NSApplicationSupportDirectory
                                                inDomains:NSUserDomainMask];
            if ([appSupportDir count] > 0)
            {
                // Append the bundle ID to the URL for the
                // Application Support directory
                dirPath = [[appSupportDir objectAtIndex:0] URLByAppendingPathComponent:bundleID];
                
                // If the directory does not exist, this method creates it.
                // This method is only available in OS X v10.7 and iOS 5.0 or later.
                NSError*    theError = nil;
                if (![fm createDirectoryAtURL:dirPath withIntermediateDirectories:YES
                                   attributes:nil error:&theError])
                {
                    // Handle the error.
                    
                    return nil;
                }
            }
            
            return dirPath.fileSystemRepresentation;
        }
	}
    
    namespace operations
    {
        void blit(const ImageData &src, ImageData &dest);
        void blitPremultiply(const ImageData &src, ImageData &dest);
    }
    
#if TARGET_OS_IPHONE
    bool nsImageLoad(MemoryTexture &mt, void * pData, int nDatalen, bool premultiplied, TextureFormat destFormat)
    {
        NSData *data = [NSData dataWithBytesNoCopy:pData length:nDatalen];
        UIImage *image = [UIImage imageWithData:data];
        
        GLuint width = CGImageGetWidth(image.CGImage);
        GLuint height = CGImageGetHeight(image.CGImage);
        
        int bits = CGImageGetBitsPerPixel(image.CGImage);
        int pitch = CGImageGetBytesPerRow(image.CGImage);
        
        CGImageAlphaInfo alpha = CGImageGetAlphaInfo(image.CGImage);
        TextureFormat srcFormat = bits == 32 ? TF_R8G8B8A8 : TF_R8G8B8;
        
        if (destFormat == TF_UNDEFINED)
            destFormat = srcFormat;

        
        MemoryTexture temp;
        temp.init(width, height, srcFormat);
        
        
        ImageData src = temp.lock();
        
        CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
        CGContextRef imgcontext = CGBitmapContextCreate( src.data, width, height, 8, src.pitch, colorSpace, kCGImageAlphaPremultipliedLast | kCGBitmapByteOrder32Big );
        CGColorSpaceRelease(colorSpace);
        CGContextClearRect(imgcontext, CGRectMake( 0, 0, width, height ) );
        //CGContextTranslateCTM( imgcontext, 0, height - height );
        CGContextDrawImage(imgcontext, CGRectMake( 0, 0, width, height ), image.CGImage );
        
        CGContextRelease(imgcontext);
        
        temp.unlock();

        mt.init(width, height, destFormat);
        ImageData dest = mt.lock();
        
        if (!premultiplied)
        {
            operations::blitPremultiply(src, dest);
        }
        else
        {
            operations::blit(src, dest);
        }
        
        mt.unlock();
        
        
        return true;
    }
#else
    bool nsImageLoad(MemoryTexture &mt, void * pData, int nDatalen, bool premultiplied, TextureFormat destFormat)
    {
        NSData *data = [NSData dataWithBytesNoCopy:pData length:nDatalen];
        NSBitmapImageRep *image = [NSBitmapImageRep imageRepWithData:data];
        int w = image.size.width;
        int h = image.size.height;
        
        TextureFormat srcFormat = image.bitsPerPixel == 32 ? TF_R8G8B8A8 : TF_R8G8B8;
        ImageData src(w, h, (int) image.bytesPerRow, srcFormat, image.bitmapData);
        
        if (destFormat == TF_UNDEFINED)
            destFormat = srcFormat;
        
        mt.init(w, h, destFormat);
        ImageData dest = mt.lock();
        
        if (!premultiplied)
        {
            operations::blitPremultiply(src, dest);
        }
        else
        {
            operations::blit(src, dest);
        }
        
        mt.unlock();
        
        
        return true;
    }
#endif
}