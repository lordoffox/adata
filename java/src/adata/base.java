package adata;

public interface Base {
    public abstract void read(adata.Stream stream) throws Exception;
    public default void skip_read(adata.Stream stream) throws Exception{
    	stream.skip_read_compatible();
    }
    public abstract int size_of();
    public abstract void write(adata.Stream stream)throws Exception;
    public abstract void raw_read(adata.Stream stream) throws Exception;
    public abstract int raw_size_of();
    public abstract void raw_write(adata.Stream stream)throws Exception;
}
