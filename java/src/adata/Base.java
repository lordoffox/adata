package adata;

public interface Base {
    public abstract void read(adata.Stream stream) throws Exception;
    public default void skipRead(adata.Stream stream) throws Exception{
    	stream.skipReadCompatible();
    }
    public abstract int sizeOf();
    public abstract void write(adata.Stream stream)throws Exception;
    public abstract void rawRead(adata.Stream stream) throws Exception;
    public abstract int rawSizeOf();
    public abstract void rawWrite(adata.Stream stream)throws Exception;
}
