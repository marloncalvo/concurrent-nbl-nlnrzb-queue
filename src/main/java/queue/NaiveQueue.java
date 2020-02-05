import java.util.LinkedList;

public class NaiveQueue<T> implements Queue<T> {

	private final LinkedList<T> items = new LinkedList<>();

	public void add(T item) {
		items.add(item);
	}

	public T poll() {
		return items.remove();
	}

	public boolean isEmpty() {
		return items.isEmpty();
	}
}