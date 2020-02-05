import java.util.LinkedList;

public class NaiveQueue<T> implements Queue<T> {

	private final LinkedList<T> items = new LinkedList<>();

	// For LL, this will always add to the end of the list
	// as O(1) operation.
	public void add(T item) {
		items.add(item);
	}

	// For LL, removes from head of list, as O(1) operation.
	public T poll() {
		return items.remove();
	}

	public boolean isEmpty() {
		return items.isEmpty();
	}
}