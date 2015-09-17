Q: How does phrasing work?

A: It works independent of parsing.  There are so called terminal parsers
   which have no class templates. They do not return anything else than
   bool. You can simply find out what they parsed by comparing the iterators
   before and afterwards.

   Next, on success, your phraser converts the found substring into a
   return value for this parser. These return values must be convertible to
   bool.

Q: Why static functions? I.e.: Why not containing parsers in others?

A: This would make sense if the parsers would store the results. However,
   e.g. for kleenee, this would mean storing and reallocating a vector, even
   if you do not want to phrase, just parse. Also, should kleenee conatain a
   vector or a number?

Q: Why the strange mangling with the debug classes?

A: In order to return the parsed data (e.g. AST, numeric results) type-safely,
   we wanted to store the result in a class that could be chosen. A void
   pointer and simple casts would have sufficed, but then, again, casts to
   what type?
   
   Also, the results needed to be combined in some way. How could this be
   better done than that way?
   
   To cut a long story short, there needed to be some type info passed to all
   the tree-aligned (sub-)parsers. Instead of giving them another annoying
   template, the sub-parser template could be exploited.

   
