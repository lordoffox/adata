package adata;

public abstract class Base {
    public abstract void read(adata.Stream stream);
    public void skipRead(adata.Stream stream){
    	stream.skipReadCompatible();
    }
    public abstract int sizeOf();
    public abstract void write(adata.Stream stream);
    public abstract void rawRead(adata.Stream stream);
    public abstract int rawSizeOf();
    public abstract void rawWrite(adata.Stream stream);
}
