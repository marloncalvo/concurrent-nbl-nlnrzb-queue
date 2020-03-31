package qqueue;

import java.util.concurrent.atomic.AtomicInteger;

public class MutableInt {
    public final AtomicInteger value = new AtomicInteger();
}
