public interface Queue<T> {

	public void add(T item);
	public T poll();
	public boolean isEmpty();
	
}