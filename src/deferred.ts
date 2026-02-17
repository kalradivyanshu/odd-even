type DeferredState = "pending" | "resolved" | "rejected";

export class Deferred<T> {
  public promise: Promise<T>;
  public resolve!: (value: T) => void;
  public reject!: (reason?: unknown) => void;

  private state: DeferredState = "pending";
  private value?: T;

  constructor() {
    this.promise = new Promise<T>((resolve, reject) => {
      this.resolve = (value: T) => {
        this.state = "resolved";
        this.value = value;
        resolve(value);
      };
      this.reject = (reason?: unknown) => {
        this.state = "rejected";
        reject(reason);
      };
    });
  }

  is_pending(): boolean {
    return this.state === "pending";
  }

  is_resolved(): boolean {
    return this.state === "resolved";
  }

  is_rejected(): boolean {
    return this.state === "rejected";
  }

  static resolved<T>(data: T): Deferred<T> {
    const deferred = new Deferred<T>();
    deferred.resolve(data);
    return deferred;
  }

  async(): Promise<T> {
    return this.promise;
  }

  sync(): T | undefined {
    return this.value;
  }
}
