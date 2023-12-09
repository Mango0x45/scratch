{ _, data } = File.read "input"

xs = String.split(data, "\n\n")
|> Enum.map(fn x ->
	String.split(x)
	|> Enum.reduce(0, &(&2 + elem(Integer.parse(&1), 0)))
end)

IO.puts Enum.max(xs)
IO.puts Enum.sort(xs, :desc)
|> Enum.take(3)
|> Enum.sum
